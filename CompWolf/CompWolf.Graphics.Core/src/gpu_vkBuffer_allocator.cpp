#include "pch.h"
#include "gpu_buffers"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include <compwolf_utility>
#include "gpus"

namespace CompWolf
{
	/******************************** other methods ********************************/

	void Private::base_gpu_vkBuffer_buffer::bind_to_shader(gpu_memory_bind_data* bind_ptr) const
	{
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());
		auto vkBuffer = Private::to_vulkan(vulkan_buffer());
		auto& bind_data = *bind_ptr;

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

	/******************************** CompWolf::base_gpu_buffer_allocator ********************************/

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(VkBuffer, VkBuffer, base_gpu_buffer_allocator::data_handle)

	auto gpu_vkBuffer_allocator::from_buffer(Private::vulkan_buffer data) const noexcept -> data_handle
	{
		return from_VkBuffer(Private::to_vulkan(data));
	}
	auto gpu_vkBuffer_allocator::to_buffer(data_handle data) const noexcept -> Private::vulkan_buffer
	{
		return Private::from_vulkan(to_VkBuffer(data));
	}

	auto gpu_vkBuffer_allocator::alloc_data() const -> data_handle
	{
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

		VkBuffer buffer;
		{
			VkBufferCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = static_cast<VkDeviceSize>(size() * element_stride()),
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			};
			switch (type())
			{
			case gpu_vkBuffer_type::index: createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
			case gpu_vkBuffer_type::input: createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
			case gpu_vkBuffer_type::uniform: createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
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
	auto gpu_vkBuffer_allocator::alloc_cpu_access(data_handle a) const -> data_handle
	{
		// !!! This is unused
		return a;
	}

	void gpu_vkBuffer_allocator::bind_data(data_handle data, Private::vulkan_memory memory) const
	{
		vkBindBufferMemory(Private::to_vulkan(gpu().vulkan_device())
			, to_VkBuffer(data)
			, Private::to_vulkan(memory)
			, 0
		);
	}

	void gpu_vkBuffer_allocator::free_cpu_access(data_handle data) const noexcept {}
	void gpu_vkBuffer_allocator::free_data(data_handle data) const noexcept
	{
		vkDestroyBuffer(Private::to_vulkan(gpu().vulkan_device())
			, to_VkBuffer(data)
			, nullptr
		);
	}

	void gpu_vkBuffer_allocator::private_info(data_handle data, Private::gpu_buffer_private_info* out_pointer) const
	{
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());
		auto buffer = to_VkBuffer(data);
		auto& out = *out_pointer;

		out.size = size();
		vkGetBufferMemoryRequirements(logicDevice, buffer, &out.memoryRequirements);
	}
}
