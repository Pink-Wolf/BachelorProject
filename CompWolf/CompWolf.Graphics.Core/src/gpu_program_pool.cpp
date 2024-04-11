#include "pch.h"
#include "gpu_program"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/
	gpu_program_pool::gpu_program_pool(gpu_connection& target_device, size_t thread_family_index, size_t thread_index)
		: _device(&target_device)
		, _thread_family_index(thread_family_index)
		, _thread_index(thread_index)
		, _vulkan_pool(nullptr)
	{
		std::ignore = target_device.families().at(thread_family_index); // throw if index is not within array.

		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = static_cast<uint32_t>(thread_family_index),
		};

		VkCommandPool commandPool;
		auto result = vkCreateCommandPool(logicDevice, &createInfo, nullptr, &commandPool);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not set up command pool.");
		}

		_vulkan_pool = Private::from_vulkan(commandPool);
	}
	/******************************** CompWolf::freeable ********************************/

	void gpu_program_pool::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		vkDeviceWaitIdle(logicDevice);

		vkDestroyCommandPool(logicDevice, Private::to_vulkan(_vulkan_pool), nullptr);

		_device = nullptr;
	}
}
