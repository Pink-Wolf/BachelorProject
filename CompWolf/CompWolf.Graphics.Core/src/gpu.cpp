#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <compwolf_functional>

namespace CompWolf::Graphics
{
	gpu::gpu(Private::vulkan_physical_device* vulkan_physical_device)
	{
		auto physical_device = Private::to_vulkan(vulkan_physical_device);

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physical_device, &features);

		const float one = 1.f;
		std::vector<float> ones(8, one);

		auto queue_families = Private::get_size_and_vector<VkPhysicalDevice, uint32_t, VkQueueFamilyProperties, void>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		queue_create_infos.reserve(queue_families.size());
		std::transform(queue_families.begin(), queue_families.end(), std::back_inserter(queue_create_infos),
			index_function<VkDeviceQueueCreateInfo(VkQueueFamilyProperties)>([&one, &ones](int index, VkQueueFamilyProperties queue_family)->VkDeviceQueueCreateInfo
				{
					auto is_graphics_family = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;

					auto queue_count = queue_family.queueCount;
					if (ones.size() < queue_count) ones.resize(queue_count, one);

					VkDeviceQueueCreateInfo queue_create_info{
						.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
						.queueFamilyIndex = static_cast<uint32_t>(index),
						.queueCount = queue_family.queueCount,
						.pQueuePriorities = ones.data(),
					};

					return queue_create_info;
				}
			)
		);

		VkDeviceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
			.pQueueCreateInfos = queue_create_infos.data(),
		};

		VkDevice logic_device;
		auto result = vkCreateDevice(physical_device, &create_info, nullptr, &logic_device);

		switch (result)
		{
		case VK_SUCCESS: break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: throw std::runtime_error("Could not set up a connection to a gpu; the machine does not have the right extensions.");
		case VK_ERROR_FEATURE_NOT_PRESENT: throw std::runtime_error("Could not set up a connection to a gpu; the machine does not have the right features.");
		default: throw std::runtime_error("Could not set up a connection to a gpu.");
		}

		vulkan_device = Private::from_vulkan(logic_device);
	}
	gpu::~gpu()
	{
		vkDestroyDevice(Private::to_vulkan(vulkan_device), nullptr);
	}
}
