#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"

namespace CompWolf::Graphics
{
	void Private::compile_bind_vertex_command(base_gpu_buffer* buffer, const gpu_command_compile_settings& settings)
	{
		auto command = Private::to_vulkan(settings.command);
		auto vkBuffer = Private::to_vulkan(buffer->vulkan_buffer());
		static VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(command, 0, 1, &vkBuffer, offsets);
	}
	void Private::compile_bind_uniform_command(base_gpu_buffer* buffer, const gpu_command_compile_settings& settings)
	{
		auto command = Private::to_vulkan(settings.command);
		auto vkBuffer = Private::to_vulkan(buffer->vulkan_buffer());
		auto descriptorSet = Private::to_vulkan(settings.pipeline->vulkan_descriptor_sets()[settings.frame_index]);
		auto logicDevice = Private::to_vulkan(settings.pipeline->target_window().device().vulkan_device());

		VkDescriptorBufferInfo bufferInfo{
			.buffer = vkBuffer,
			.offset = 0,
			.range = static_cast<VkDeviceSize>(buffer->size()),
		};
		VkWriteDescriptorSet writer{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &bufferInfo,
		};

		vkUpdateDescriptorSets(logicDevice, 1, &writer, 0, nullptr);

		vkCmdBindDescriptorSets(command
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, Private::to_vulkan(settings.pipeline->gpu_data().layout())
			, 0
			, 1
			, &descriptorSet
			, 0
			, nullptr
		);
	}

	void draw_command::compile(const gpu_command_compile_settings& settings)
	{
		auto command = Private::to_vulkan(settings.command);
		auto indexBuffer = Private::to_vulkan(indices().vulkan_buffer());

		vkCmdBindIndexBuffer(command, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command, static_cast<uint32_t>(indices().size()), 1, 0, 0, 0);
	}
}
