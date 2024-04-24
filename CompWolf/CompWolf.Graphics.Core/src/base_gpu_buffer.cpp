#include "pch.h"
#include "gpu_buffers"

#include "compwolf_vulkan.hpp"
#include "gpus"
#include "gpu_memory_type.hpp"
#include <compwolf_utility>

namespace CompWolf::Graphics
{
	/******************************** constructor ********************************/

	base_gpu_buffer::base_gpu_buffer(base_gpu_buffer_allocator* allocator)
		: _allocator(allocator)
		, _vulkan_data(nullptr), _vulkan_memory(nullptr)
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto physicalDevice = Private::to_vulkan(device().vulkan_physical_device());

		try
		{
			_vulkan_data = allocator->alloc_data();

			Private::gpu_buffer_private_info data_info;
			{
				allocator->private_info(_vulkan_data, &data_info);
				_item_count = data_info.size;
				_memory_size = data_info.memoryRequirements.size;
			}

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

					if ((data_info.memoryRequirements.memoryTypeBits & (1_uint32 << heap_index)) == 0) continue;
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
					.allocationSize = data_info.memoryRequirements.size,
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

			allocator->bind_data(_vulkan_data, _vulkan_memory);
		}
		catch (...)
		{
			free();
			throw;
		}

	}

	Private::base_gpu_buffer_access::base_gpu_buffer_access(base_gpu_buffer* target)
		: _buffer(target)
	{
		if (target == nullptr) throw std::invalid_argument("base_gpu_buffer_access's constructor was given a nullptr");

		auto logicDevice = Private::to_vulkan(target_buffer().device().vulkan_device());
		auto vkMemory = Private::to_vulkan(target_buffer().vulkan_memory());
		auto memorySize = static_cast<VkDeviceSize>(target_buffer().vulkan_memory_size());

		vkMapMemory(logicDevice, vkMemory, 0, memorySize, 0, &_data);
	}

	/******************************** CompWolf::freeable ********************************/

	void base_gpu_buffer::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto memory = Private::to_vulkan(_vulkan_memory);

		if (memory) vkFreeMemory(logicDevice, memory, nullptr);
		if (_vulkan_data) _allocator->free_data(_vulkan_data);

		vulkan_allocator()->free();
	}

	void Private::base_gpu_buffer_access::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(target_buffer().device().vulkan_device());
		auto vkMemory = Private::to_vulkan(target_buffer().vulkan_memory());

		vkUnmapMemory(logicDevice, vkMemory);

		_buffer = nullptr;
	}
}
