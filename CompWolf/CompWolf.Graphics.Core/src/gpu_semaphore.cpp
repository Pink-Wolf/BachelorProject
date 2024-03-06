#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	gpu_semaphore::gpu_semaphore(const gpu& target_gpu) : basic_const_gpu_user(target_gpu)
	{
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
	void gpu_semaphore::clear() noexcept
	{
		if (empty()) return;

		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		vkDestroySemaphore(vulkan_device, Private::to_vulkan(_vulkan_semaphore), nullptr);
	}

	gpu_semaphore::gpu_semaphore(gpu_semaphore&& other)
	{
		set_device(other.device());
		_vulkan_semaphore = std::move(other._vulkan_semaphore);

		other._vulkan_semaphore = nullptr;
	}
	auto gpu_semaphore::operator=(gpu_semaphore&& other) -> gpu_semaphore&
	{
		set_device(other.device());
		_vulkan_semaphore = std::move(other._vulkan_semaphore);

		other._vulkan_semaphore = nullptr;

		return *this;
	}
}
