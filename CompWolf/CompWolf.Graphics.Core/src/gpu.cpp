#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <compwolf_functional>
#include <string.h>
#include <set>
#include <map>

namespace CompWolf::Graphics
{
	gpu::gpu(Private::vulkan_physical_device vulkan_physical_device)
	{
		auto physical_device = Private::to_vulkan(vulkan_physical_device);

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physical_device, &features);

		auto extension_properties = Private::get_size_and_vector<VkPhysicalDevice, uint32_t, VkExtensionProperties, VkResult>(
			[](VkPhysicalDevice a, uint32_t* b, VkExtensionProperties* c) { return vkEnumerateDeviceExtensionProperties(a, nullptr, b, c); },
			physical_device, [](VkResult result)
			{
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default: throw std::runtime_error("Could not get extensions from gpu");
				}
			}
		);
		auto extension_properties_names_vector = extension_properties | std::views::transform([](VkExtensionProperties a)->const char* { return a.extensionName; });
		std::set <
			const char*,
			decltype([](const char* const& a, const char* const& b) { return strcmp(a, b) < 0; })
		>
			extension_properties_names(extension_properties_names_vector.begin(), extension_properties_names_vector.end());

		std::vector<const char*> enabled_extensions;
		bool is_present_device;
		{
			is_present_device = extension_properties_names.contains(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			if (is_present_device) enabled_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		const float queue_priority_item = .5f;
		std::vector<float> queue_priority(8, queue_priority_item);
		queue_priority[0] = 1.f; // Include one high-priority queue

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		{
			auto queue_families = Private::get_size_and_vector<VkPhysicalDevice, uint32_t, VkQueueFamilyProperties, void>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
			queue_create_infos.reserve(queue_families.size());
			std::transform(queue_families.begin(), queue_families.end(), std::back_inserter(queue_create_infos),
				index_function<VkDeviceQueueCreateInfo(VkQueueFamilyProperties)>([&queue_priority_item, &queue_priority](int index, VkQueueFamilyProperties queue_family)->VkDeviceQueueCreateInfo
					{
						auto is_graphics_family = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;

						auto queue_count = queue_family.queueCount;
						if (queue_priority.size() < queue_count) queue_priority.resize(queue_count, queue_priority_item);

						VkDeviceQueueCreateInfo queue_create_info{
							.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
							.queueFamilyIndex = static_cast<uint32_t>(index),
							.queueCount = queue_family.queueCount,
							.pQueuePriorities = queue_priority.data(),
						};

						return queue_create_info;
					}
				)
			);
		}

		VkDeviceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
			.pQueueCreateInfos = queue_create_infos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
			.ppEnabledExtensionNames = enabled_extensions.data(),
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

		_vulkan_device = Private::from_vulkan(logic_device);
	}
	gpu::~gpu()
	{
		if (_vulkan_device) vkDestroyDevice(Private::to_vulkan(_vulkan_device), nullptr);
	}

	gpu::gpu(gpu&& other) noexcept
	{
		_vulkan_device = other._vulkan_device;
		other._vulkan_device = nullptr;
	}
	gpu& gpu::operator=(gpu&& other) noexcept
	{
		_vulkan_device = other._vulkan_device;
		other._vulkan_device = nullptr;
		return *this;
	}
}
