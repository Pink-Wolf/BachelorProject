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
	gpu::gpu(Private::vulkan_instance vulkan_instance, Private::vulkan_physical_device vulkan_physical_device)
	{
		auto instance = Private::to_vulkan(vulkan_instance);
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
			_families.reserve(queue_families.size());

			for (size_t queue_index = 0; queue_index < queue_families.size(); queue_index++)
			{
				auto& queue_family = queue_families[queue_index];

				gpu_thread_family connection;

				bool draw_queue = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
				if (draw_queue) connection.work_types[gpu_work_type::draw] = true;
				bool present_queue = glfwGetPhysicalDevicePresentationSupport(instance, physical_device, static_cast<uint32_t>(queue_index));
				if (present_queue) connection.work_types[gpu_work_type::present] = true;

				connection.size = queue_family.queueCount;
				if (queue_priority.size() < connection.size) queue_priority.resize(connection.size, queue_priority_item);

				VkDeviceQueueCreateInfo queue_create_info{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = static_cast<uint32_t>(queue_index),
					.queueCount = static_cast<uint32_t>(connection.size),
					.pQueuePriorities = queue_priority.data(),
				};

				queue_create_infos.push_back(std::move(queue_create_info));
				_families.push_back(std::move(connection));
			}
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
