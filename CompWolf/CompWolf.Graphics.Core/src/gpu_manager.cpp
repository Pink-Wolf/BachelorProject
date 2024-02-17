#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>

namespace CompWolf::Graphics
{
	gpu_manager::gpu_manager(const graphics_environment_settings&, Private::vulkan_instance vulkan_instance)
	{
		auto instance = Private::to_vulkan(vulkan_instance);

		auto physical_devices = Private::get_size_and_vector<VkInstance, uint32_t, VkPhysicalDevice, VkResult>(vkEnumeratePhysicalDevices, instance, [](VkResult result)
			{
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default: throw std::runtime_error("Could not get the machine's graphics card");
				}
			});

		_gpus.reserve(physical_devices.size());
		for (auto& physical_device : physical_devices)
		{
			auto vulkan_device = Private::from_vulkan(physical_device);
			_gpus.push_back(gpu(vulkan_instance, vulkan_device));
		}
	}
}
