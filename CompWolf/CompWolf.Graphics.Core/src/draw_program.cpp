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

	auto frame_draw_program::new_frameset(window_specific_pipeline& pipeline, gpu_command* command) -> frameset_type
	{
		auto size = pipeline.target_window().swapchain().frames().size();
		frameset_type data;
		data.reserve(size);
		for (std::size_t i = 0; i < size; ++i)
		{
			data.emplace_back(pipeline, command, i);
		}
		return data;
	}
	void frame_draw_program::on_compile(window_specific_pipeline* pipeline, gpu_command* command, std::size_t index, const gpu_program_compile_parameter& args)
	{
		auto commandBuffer = Private::to_vulkan(args.command);

		VkClearValue clearColor = { {{.0f, .0f, .0f, .0f}} };
		uint32_t width, height;
		{
			auto size = pipeline->target_window().pixel_size().value();
			width = static_cast<uint32_t>(size.first);
			height = static_cast<uint32_t>(size.second);
		}

		VkRenderPassBeginInfo renderpassInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = Private::to_vulkan(pipeline->vulkan_render_pass()),
			.framebuffer = Private::to_vulkan(pipeline->vulkan_frame_buffer(index)),
			.renderArea = {
				.offset = {0, 0},
				.extent = {
					.width = width,
					.height = height,
				},
			},
			.clearValueCount = 1,
			.pClearValues = &clearColor,
		};

		vkCmdBeginRenderPass(commandBuffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Private::to_vulkan(pipeline->vulkan_pipeline()));
			VkViewport viewport{
				.x = .0f,
				.y = .0f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = .0f,
				.maxDepth = 1.f,
			};
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &renderpassInfo.renderArea);

			gpu_command_compile_settings command_settings{
				.command = Private::from_vulkan(commandBuffer),
				.pipeline = pipeline,
				.frame_index = index,
			};
			command->compile(command_settings);
		}
		vkCmdEndRenderPass(commandBuffer);
	}
}
