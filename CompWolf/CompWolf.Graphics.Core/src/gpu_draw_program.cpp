#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <compwolf_utility>

namespace CompWolf::Graphics::Private
{
	/******************************** constructors ********************************/

	auto gpu_draw_program_frame_data::new_frameset(window_specific_pipeline& pipeline, gpu_command* command) -> frameset_type
	{
		auto size = pipeline.target_window().swapchain().frames().size();
		frameset_type data;
		data.reserve(size);
		for (size_t i = 0; i < size; ++i)
		{
			data.emplace_back(pipeline, command, i);
		}
		return data;
	}
	gpu_draw_program_frame_data::gpu_draw_program_frame_data(window_specific_pipeline& pipeline, gpu_command* command, size_t index)
	{
		_pipeline = &pipeline;
		_index = index;
		auto& gpu_device = target_window().device();
		auto& family = target_window().draw_present_job().family();
		auto device = Private::to_vulkan(gpu_device.vulkan_device());
		auto& frame = target_window().swapchain().frames()[index];

		try
		{
			VkCommandBuffer command_buffer;
			{
				VkCommandBufferAllocateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.commandPool = Private::to_vulkan(frame.command_pool),
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1,
				};

				auto result = vkAllocateCommandBuffers(device, &create_info, &command_buffer);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"command buffer\" for gpu commands.");
				}

				_vulkan_command = Private::from_vulkan(command_buffer);
			}

			{
				{
					VkCommandBufferBeginInfo begin_info{
						.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
					};

					auto result = vkBeginCommandBuffer(command_buffer, &begin_info);

					switch (result)
					{
					case VK_SUCCESS: break;
					default: throw std::runtime_error("Could not begin creating command for gpu commands.");
					}
				}

				{
					VkClearValue clear_color = { {{.0f, .0f, .0f, .0f}} };
					uint32_t width, height;
					{
						auto size = target_window().pixel_size().value();
						width = static_cast<uint32_t>(size.first);
						height = static_cast<uint32_t>(size.second);
					}

					VkRenderPassBeginInfo renderpass_info{
						.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
						.renderPass = Private::to_vulkan(_pipeline->vulkan_render_pass()),
						.framebuffer = Private::to_vulkan(_pipeline->vulkan_frame_buffer(index)),
						.renderArea = {
							.offset = {0, 0},
							.extent = {
								.width = width,
								.height = height,
							},
						},
						.clearValueCount = 1,
						.pClearValues = &clear_color,
					};

					vkCmdBeginRenderPass(command_buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
					{
						vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Private::to_vulkan(_pipeline->vulkan_pipeline()));
						VkViewport viewport{
							.x = .0f,
							.y = .0f,
							.width = static_cast<float>(width),
							.height = static_cast<float>(height),
							.minDepth = .0f,
							.maxDepth = 1.f,
						};
						vkCmdSetViewport(command_buffer, 0, 1, &viewport);
						vkCmdSetScissor(command_buffer, 0, 1, &renderpass_info.renderArea);

						command->compile(Private::from_vulkan(command_buffer));
					}
					vkCmdEndRenderPass(command_buffer);
				}

				{
					auto result = vkEndCommandBuffer(command_buffer);

					switch (result)
					{
					case VK_SUCCESS: break;
					default: throw std::runtime_error("Could not create command for gpu commands.");
					}
				}
			}
		}
		catch (...)
		{
			free();

			throw;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_draw_program_frame_data::free() noexcept
	{
		if (empty()) return;

		auto& gpu_device = target_window().device();
		auto& family = target_window().draw_present_job().family();
		auto device = Private::to_vulkan(gpu_device.vulkan_device());

		vkDeviceWaitIdle(device);

		auto command = Private::to_vulkan(_vulkan_command);
		vkFreeCommandBuffers(device, Private::to_vulkan(vulkan_command_pool()), 1, &command);
	}

	/******************************** other methods ********************************/

	void gpu_draw_program_frame_data::draw()
	{
		auto& gpu_device = target_window().device();
		auto& family = target_window().draw_present_job().family();
		auto& thread = target_window().draw_present_job().thread();
		auto queue = Private::to_vulkan(thread.queue);
		auto device = Private::to_vulkan(gpu_device.vulkan_device());
		auto swapchain = Private::to_vulkan(target_window().swapchain().vulkan_swapchain());
		uint32_t index = static_cast<uint32_t>(_index);

		auto& frame = target_window().swapchain().frames()[_index];
		auto old_semaphore = Private::to_vulkan(frame.last_vulkan_semaphore());
		auto& sync = frame.synchronizations.emplace_back(gpu_fence(gpu_device), gpu_semaphore(gpu_device));
		auto fence = Private::to_vulkan(sync.first.vulkan_fence());
		auto semaphore = Private::to_vulkan(sync.second.vulkan_semaphore());

		auto vulkan_command = Private::to_vulkan(_vulkan_command);

		{
			VkPipelineStageFlags wait_stages[]{
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			};
			VkSubmitInfo submit_info{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = (old_semaphore == nullptr) ? 0_uint32 : 1_uint32,
				.pWaitSemaphores = &old_semaphore,
				.pWaitDstStageMask = wait_stages,
				.commandBufferCount = 1,
				.pCommandBuffers = &vulkan_command,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &semaphore,
			};

			auto result = vkQueueSubmit(queue, 1, &submit_info, fence);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not submit commands to gpu.");
			}
		}
	}
}
