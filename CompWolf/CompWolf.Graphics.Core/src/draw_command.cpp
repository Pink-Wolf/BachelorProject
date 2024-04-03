#include "pch.h"
#include "draw_program"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"

namespace CompWolf::Graphics
{
	/******************************** operators ********************************/

	void Private::window_draw_command::operator()(const draw_program_input& args)
	{
		auto command = Private::to_vulkan(args.command);
		auto logicDevice = Private::to_vulkan(args.target_window->device().vulkan_device());

		{
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, Private::to_vulkan(window_pipeline().vulkan_pipeline()));

			auto& [width, height] = args.target_window->pixel_size().value();
			VkViewport viewport{
				.x = .0f,
				.y = .0f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = .0f,
				.maxDepth = 1.f,
			};
			vkCmdSetViewport(command, 0, 1, &viewport);

			VkRect2D renderArea{
				.offset = {0, 0},
				.extent = {
					.width = static_cast<uint32_t>(width),
					.height = static_cast<uint32_t>(height),
				},
			};
			vkCmdSetScissor(command, 0, 1, &renderArea);
		}
		{
			auto vkBuffer = Private::to_vulkan(_data->vertices->vulkan_buffer());
			static VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(command, 0, 1, &vkBuffer, offsets);
		}
		for (size_t i = 0; i < _data->uniform_vertex_data.size(); ++i)
		{
			auto& memory = *_data->uniform_vertex_data[i];

			Private::gpu_memory_bind_data bind_data{
				.commandBuffer = command,
				.descriptorSet = Private::to_vulkan(window_pipeline().vulkan_descriptor_sets()[args.frame_index]),
				.bindingIndex = static_cast<uint32_t>(i),
			};

			memory.bind_to_shader(Private::from_private(&bind_data));

			vkCmdBindDescriptorSets(command
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, Private::to_vulkan(window_pipeline().gpu_data().layout())
				, 0
				, 1
				, &bind_data.descriptorSet
				, 0
				, nullptr
			);
		}
		{
			auto indexBuffer = Private::to_vulkan(_data->indices->vulkan_buffer());

			vkCmdBindIndexBuffer(command, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		for (size_t i = 0; i < _data->uniform_fragment_data.size(); ++i)
		{
			auto& memory = *_data->uniform_fragment_data[i];

			Private::gpu_memory_bind_data bind_data{
				.commandBuffer = command,
				.descriptorSet = Private::to_vulkan(window_pipeline().vulkan_descriptor_sets()[args.frame_index]),
				.bindingIndex = static_cast<uint32_t>(i + _data->uniform_vertex_data.size()),
			};

			memory.bind_to_shader(Private::from_private(&bind_data));

			vkCmdBindDescriptorSets(command
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, Private::to_vulkan(window_pipeline().gpu_data().layout())
				, 0
				, 1
				, &bind_data.descriptorSet
				, 0
				, nullptr
			);
		}

		vkCmdDrawIndexed(command, static_cast<uint32_t>(_data->indices->size()), 1, 0, 0, 0);
	}
}
