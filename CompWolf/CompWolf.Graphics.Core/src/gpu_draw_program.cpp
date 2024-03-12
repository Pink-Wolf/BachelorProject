#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <stdexcept>
#include <algorithm>
#include <limits>

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

		frame().drawing_fence.wait();

		auto& gpu_device = target_window().device();
		auto& family = target_window().draw_present_job().family();
		auto device = Private::to_vulkan(gpu_device.vulkan_device());

		vkDeviceWaitIdle(device);

		auto command = Private::to_vulkan(_vulkan_command);
		vkFreeCommandBuffers(device, Private::to_vulkan(vulkan_command_pool()), 1, &command);
	}

	/******************************** other methods ********************************/

	auto gpu_draw_program_frame_data::next_frame(frameset_type& frameset) -> gpu_draw_program_frame_data&
	{
		auto& target_window = frameset[0]._pipeline->target_window();
		auto device = Private::to_vulkan(target_window.device().vulkan_device());
		auto swapchain = Private::to_vulkan(target_window.swapchain().vulkan_swapchain());

		uint32_t index;
		gpu_semaphore semaphore(target_window.device());
		auto result = vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), Private::to_vulkan(semaphore.vulkan_semaphore()), VK_NULL_HANDLE, &index);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not get next frame.");
		}

		auto& frame_data = frameset[static_cast<size_t>(index)];
		auto& frame = frame_data.frame();
		
		frame.drawing_fence.wait();
		frame.drawing_semaphore = std::move(semaphore);

		return frame_data;
	}

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
		auto frame_fence = Private::to_vulkan(frame.drawing_fence.vulkan_fence());
		auto frame_semaphore = Private::to_vulkan(frame.drawing_semaphore.vulkan_semaphore());

		auto vulkan_command = Private::to_vulkan(_vulkan_command);

		frame.drawing_fence.wait();
		vkResetFences(device, 1, &frame_fence);

		{
			VkPipelineStageFlags wait_stages[]{
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			};
			VkSubmitInfo submit_info{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &frame_semaphore,
				.pWaitDstStageMask = wait_stages,
				.commandBufferCount = 1,
				.pCommandBuffers = &vulkan_command,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &frame_semaphore,
			};

			auto result = vkQueueSubmit(queue, 1, &submit_info, Private::to_vulkan(frame.drawing_fence.vulkan_fence()));

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not submit commands to gpu.");
			}
		}

		{
			VkPresentInfoKHR present_info{
				.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &frame_semaphore,
				.swapchainCount = 1,
				.pSwapchains = &swapchain,
				.pImageIndices = &index,
			};

			vkQueuePresentKHR(queue, &present_info);
		}
	}
}
