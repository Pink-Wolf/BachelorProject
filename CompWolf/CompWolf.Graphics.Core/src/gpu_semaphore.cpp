#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_semaphore::gpu_semaphore(const gpu& target_gpu)
	{
		_device = &target_gpu;
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		VkSemaphoreCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};

		VkSemaphore semaphore;
		auto result = vkCreateSemaphore(vulkan_device, &create_info, nullptr, &semaphore);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not create gpu semaphore.");
		}

		_vulkan_semaphore = Private::from_vulkan(semaphore);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_semaphore::free() noexcept
	{
		if (empty()) return;

		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		vkDestroySemaphore(vulkan_device, Private::to_vulkan(_vulkan_semaphore), nullptr);

		_vulkan_semaphore = nullptr;
	}
}
