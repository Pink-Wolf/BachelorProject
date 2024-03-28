#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"

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
			auto& buffer = *_data->uniform_vertex_data[i];
			auto vkBuffer = Private::to_vulkan(buffer.vulkan_buffer());
			auto descriptorSet = Private::to_vulkan(window_pipeline().vulkan_descriptor_sets()[args.frame_index]);

			VkDescriptorBufferInfo bufferInfo{
				.buffer = vkBuffer,
				.offset = 0,
				.range = static_cast<VkDeviceSize>(buffer.size()),
			};
			VkWriteDescriptorSet writer{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSet,
				.dstBinding = static_cast<uint32_t>(i),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo,
			};

			vkUpdateDescriptorSets(logicDevice, 1, &writer, 0, nullptr);

			vkCmdBindDescriptorSets(command
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, Private::to_vulkan(window_pipeline().gpu_data().layout())
				, 0
				, 1
				, &descriptorSet
				, 0
				, nullptr
			);
		}
		{
			auto indexBuffer = Private::to_vulkan(_data->indices->vulkan_buffer());

			vkCmdBindIndexBuffer(command, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		vkCmdDrawIndexed(command, static_cast<uint32_t>(_data->indices->size()), 1, 0, 0, 0);
	}
}
