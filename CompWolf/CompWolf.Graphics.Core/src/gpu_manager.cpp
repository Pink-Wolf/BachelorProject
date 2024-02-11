#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>

namespace CompWolf::Graphics
{
	gpu_manager::gpu_manager(Private::vulkan_instance* vulkan_instance)
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
		auto vulkan_physical_devices = physical_devices | std::views::transform([](VkPhysicalDevice a) { return Private::from_vulkan(a); });

		_gpus = _gpus_type(vulkan_physical_devices.begin(), vulkan_physical_devices.end());
	}
}
