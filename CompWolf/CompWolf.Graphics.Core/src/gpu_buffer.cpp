#include "pch.h"
#include "gpu_program"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include <compwolf_utility>

namespace CompWolf::Graphics
{
	/******************************** other methods ********************************/

	void Private::base_gpu_buffer::bind_to_shader(gpu_memory::bind_handle bind_handle) const
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto vkBuffer = Private::to_vulkan(vulkan_buffer());
		auto& bind_data = *Private::to_private(bind_handle);

		VkDescriptorBufferInfo bufferInfo{
			.buffer = vkBuffer,
			.offset = 0,
			.range = static_cast<VkDeviceSize>(size()),
		};
		VkWriteDescriptorSet writer{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = bind_data.descriptorSet,
			.dstBinding = bind_data.bindingIndex,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &bufferInfo,
		};

		vkUpdateDescriptorSets(logicDevice, 1, &writer, 0, nullptr);
	}

	/******************************** CompWolf::Graphics::gpu_buffer_allocator ********************************/

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(VkBuffer, VkBuffer, Private::gpu_buffer_allocator::data_handle)

	auto Private::gpu_buffer_allocator::from_buffer(Private::vulkan_buffer data) const noexcept -> data_handle
	{
		return from_VkBuffer(to_vulkan(data));
	}
	auto Private::gpu_buffer_allocator::to_buffer(data_handle data) const noexcept -> Private::vulkan_buffer
	{
		return from_vulkan(to_VkBuffer(data));
	}

	auto Private::gpu_buffer_allocator::alloc_data() const -> data_handle
	{
		auto logicDevice = Private::to_vulkan(target_device->vulkan_device());

		VkBuffer buffer;
		{
			VkBufferCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = static_cast<VkDeviceSize>(size * element_stride),
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			};
			switch (type)
			{
			case gpu_buffer_type::index: createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
			case gpu_buffer_type::vertex: createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
			case gpu_buffer_type::uniform: createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
			default: throw std::invalid_argument("Could not create buffer on GPU; the given type is unknown.");
			}

			auto result = vkCreateBuffer(logicDevice, &createInfo, nullptr, &buffer);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_HOST_MEMORY: throw std::runtime_error("Could not create buffer on GPU; not enough CPU space.");
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw std::runtime_error("Could not create buffer on GPU; not enough GPU space.");
			default: throw std::runtime_error("Could not create buffer on GPU.");
			}
		}
		return from_VkBuffer(buffer);
	}

	void Private::gpu_buffer_allocator::bind_data(data_handle data, vulkan_memory memory) const
	{
		vkBindBufferMemory(Private::to_vulkan(target_device->vulkan_device())
			, to_VkBuffer(data)
			, Private::to_vulkan(memory)
			, 0
		);
	}

	void Private::gpu_buffer_allocator::free_data(data_handle data) const noexcept
	{
		vkDestroyBuffer(Private::to_vulkan(target_device->vulkan_device())
			, to_VkBuffer(data)
			, nullptr
		);
	}

	void Private::gpu_buffer_allocator::private_info(data_handle data, private_info_handle out_pointer) const
	{
		auto logicDevice = Private::to_vulkan(target_device->vulkan_device());
		auto buffer = to_VkBuffer(data);
		auto& out = *to_private(out_pointer);

		out.size = size;
		vkGetBufferMemoryRequirements(logicDevice, buffer, &out.memoryRequirements);
	}
}
