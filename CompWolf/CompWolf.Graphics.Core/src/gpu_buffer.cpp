#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include <compwolf_utility>

namespace CompWolf::Graphics::Private
{
	/******************************** constructor ********************************/

	base_gpu_buffer::base_gpu_buffer(gpu& target_device, size_t item_count, size_t item_stride)
		: _device(&target_device)
		, _item_count(item_count)
	{
		auto logicDevice = Private::to_vulkan(target_device.vulkan_device());
		auto physicalDevice = Private::to_vulkan(target_device.vulkan_physical_device());

		VkBuffer buffer;
		{
			VkBufferCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = item_count * item_stride,
				.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			};
			auto result = vkCreateBuffer(logicDevice, &createInfo, nullptr, &buffer);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_HOST_MEMORY: throw std::runtime_error("Could not create buffer on GPU; not enough CPU space.");
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw std::runtime_error("Could not create buffer on GPU; not enough GPU space.");
			default: throw std::runtime_error("Could not create buffer on GPU.");
			}

			_vulkan_buffer = Private::from_vulkan(buffer);
		}

		VkMemoryRequirements memoryRequirement;
		vkGetBufferMemoryRequirements(logicDevice, buffer, &memoryRequirement);
		_memory_size = memoryRequirement.size;

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		uint32_t heap_index;
		{
			VkMemoryPropertyFlags requiredProperties
				= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				;

			for (heap_index = 0; heap_index < memoryProperties.memoryTypeCount; ++heap_index)
			{
				auto& memoryType = memoryProperties.memoryTypes[heap_index];

				if ((memoryRequirement.memoryTypeBits & (1_uint32 << heap_index)) == 0) continue;
				if ((memoryType.propertyFlags & requiredProperties) != requiredProperties) continue;
				break;
			}
			if (heap_index == memoryProperties.memoryTypeCount)
				throw std::runtime_error("Could not create buffer on GPU; no suitable type of memory on the GPU.");
		}

		VkDeviceMemory memory;
		{
			VkMemoryAllocateInfo allocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = memoryRequirement.size,
				.memoryTypeIndex = heap_index,
			};
			auto result = vkAllocateMemory(logicDevice, &allocateInfo, nullptr, &memory);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_HOST_MEMORY: throw std::runtime_error("Could not allocate memory on GPU; not enough CPU space.");
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw std::runtime_error("Could not allocate memory on GPU; not enough GPU space.");
			default: throw std::runtime_error("Could not allocate memory on GPU.");
			}

			_vulkan_memory = Private::from_vulkan(memory);
		}

		vkBindBufferMemory(logicDevice, buffer, memory, 0);
	}

	base_gpu_buffer_data::base_gpu_buffer_data(base_gpu_buffer& target_buffer)
		: _buffer(&target_buffer)
	{
		auto logicDevice = Private::to_vulkan(buffer().device().vulkan_device());
		auto vkBuffer = Private::to_vulkan(buffer().vulkan_buffer());
		auto vkMemory = Private::to_vulkan(buffer().vulkan_memory());
		auto bufferSize = static_cast<VkDeviceSize>(buffer().vulkan_memory_size());

		vkMapMemory(logicDevice, vkMemory, 0, bufferSize, 0, &_data);
	}

	/******************************** CompWolf::freeable ********************************/

	void base_gpu_buffer::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto buffer = Private::to_vulkan(_vulkan_buffer);
		auto memory = Private::to_vulkan(_vulkan_memory);

		if (memory) vkFreeMemory(logicDevice, memory, nullptr);
		if (buffer) vkDestroyBuffer(logicDevice, buffer, nullptr);

		_device = nullptr;
	}

	void base_gpu_buffer_data::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(buffer().device().vulkan_device());
		auto vkBuffer = Private::to_vulkan(buffer().vulkan_buffer());
		auto vkMemory = Private::to_vulkan(buffer().vulkan_memory());

		vkUnmapMemory(logicDevice, vkMemory);

		_buffer = nullptr;
	}
}
