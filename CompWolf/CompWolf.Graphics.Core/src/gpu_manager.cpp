#include "pch.h"
#include "gpus"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>
#include <map>
#include <algorithm>
#include <limits>

namespace CompWolf
{
	/******************************** constructors ********************************/

	gpu_manager::gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance) : _thread_count(0)
	{
		auto instance = Private::to_vulkan(vulkan_instance);

		{
			auto physicalDevices = Private::get_size_and_vector<uint32_t, VkPhysicalDevice>(
				[instance](uint32_t* size, VkPhysicalDevice* data)
				{
					auto result = vkEnumeratePhysicalDevices(instance, size, data);
					switch (result)
					{
					case VK_SUCCESS:
					case VK_INCOMPLETE:
						break;
					default: throw std::runtime_error("Could not get the machine's graphics card");
					}
				}
			);

			_gpus.reserve(physicalDevices.size());
			for (auto& physical_device : physicalDevices)
			{
				auto vulkan_physical_device = Private::from_vulkan(physical_device);
				gpu_connection new_gpu(vulkan_instance, vulkan_physical_device);
				for (auto& family : new_gpu.families()) _thread_count += family.threads.size();
				_gpus.push_back(std::move(new_gpu));
			}
		}
	}
}
