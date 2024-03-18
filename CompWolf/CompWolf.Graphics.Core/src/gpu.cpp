#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <compwolf_functional>
#include <string.h>
#include <set>
#include <map>
#include <exception>

namespace CompWolf::Graphics
{
	/******************************** getters ********************************/

	auto gpu::index_of_family(const gpu_thread_family& target) const -> size_t
	{
		for (size_t i = 0; i < _families.size(); ++i)
		{
			if (&_families[i] == &target) return i;
		}
		throw std::invalid_argument("Tried getting the index of a family in a gpu, which is not actually in the gpu.");
	}

	/******************************** constructors ********************************/

	gpu::gpu(Private::vulkan_instance vulkan_instance, Private::vulkan_physical_device vulkan_physical_device) :
		_vulkan_instance(vulkan_instance),
		_vulkan_physical_device(vulkan_physical_device)
	{
		auto instance = Private::to_vulkan(vulkan_instance);
		auto physical_device = Private::to_vulkan(vulkan_physical_device);

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physical_device, &features);

		auto extension_properties = Private::get_size_and_vector<uint32_t, VkExtensionProperties>(
			[physical_device](uint32_t* size, VkExtensionProperties* data)
			{
				auto result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, size, data);
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default: throw std::runtime_error("Could not get extensions from gpu");
				}
			}
		);
		std::vector<const char*> enabled_extensions;

		// VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME depends on/includes VK_KHR_SWAPCHAIN_EXTENSION_NAME
		bool has_swapchain_extension = std::any_of(extension_properties.cbegin(), extension_properties.cend(), [](VkExtensionProperties a) { return 0 == strcmp(a.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME); });
		
		bool is_present_device = has_swapchain_extension;
		if (is_present_device)
		{
			enabled_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		const float queue_priority_item = .5f;
		std::vector<float> queue_priority(8, queue_priority_item);
		queue_priority[0] = 1.f; // Include one high-priority queue

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		{
			auto queue_families = Private::get_size_and_vector<uint32_t, VkQueueFamilyProperties>(
				[physical_device](uint32_t* size, VkQueueFamilyProperties* data) { vkGetPhysicalDeviceQueueFamilyProperties(physical_device, size, data); }
			);

			queue_create_infos.reserve(queue_families.size());
			_families.reserve(queue_families.size());

			for (size_t queue_index = 0; queue_index < queue_families.size(); queue_index++)
			{
				auto& queue_family = queue_families[queue_index];

				gpu_thread_family connection{
					.job_types = 0,
					.persistent_job_count = 0,
					.job_count = 0,
				};

				bool draw_queue = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
				if (draw_queue) connection.job_types[gpu_job_type::draw] = true;

				bool present_queue = is_present_device
					&& glfwGetPhysicalDevicePresentationSupport(instance, physical_device, static_cast<uint32_t>(queue_index));
				if (present_queue) connection.job_types[gpu_job_type::present] = true;

				auto queue_count = queue_family.queueCount;
				connection.threads.resize(queue_count);
				if (queue_priority.size() < queue_count) queue_priority.resize(queue_count, queue_priority_item);

				VkDeviceQueueCreateInfo queue_create_info{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = static_cast<uint32_t>(queue_index),
					.queueCount = static_cast<uint32_t>(queue_count),
					.pQueuePriorities = queue_priority.data(),
				};

				_work_types |= connection.job_types;
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

		for (uint32_t family_index = 0; family_index < _families.size(); ++family_index)
		{
			auto& family = _families[family_index];
			for (uint32_t thread_index = 0; thread_index < family.threads.size(); ++thread_index)
			{
				auto& thread = family.threads[thread_index];

				VkQueue queue;
				vkGetDeviceQueue(logic_device, family_index, thread_index, &queue);
				thread.queue = Private::from_vulkan(queue);
			}
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu::free() noexcept
	{
		if (empty()) return;
		vkDestroyDevice(Private::to_vulkan(_vulkan_device), nullptr);
		_vulkan_device = nullptr;
	}
}
