#include "pch.h"
#include "gpu_program_pools"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_fence::gpu_fence(const gpu_connection& target_gpu, bool signaled)
	{
		_device = &target_gpu;
		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		VkFenceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		};
		if (signaled) createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkFence fence;
		auto result = vkCreateFence(logicDevice, &createInfo, nullptr, &fence);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not create gpu fence.");
		}

		_vulkan_fence = Private::from_vulkan(fence);
	}

	bool gpu_fence::signaled() const noexcept
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		return vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, 0) == VK_SUCCESS;
	}

	/******************************** other methods ********************************/

	void gpu_fence::wait() const noexcept
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, UINT64_MAX);
	}

	void gpu_fence::reset() noexcept
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkResetFences(logicDevice, 1, &fence);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_fence::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		vkDestroyFence(logicDevice, Private::to_vulkan(_vulkan_fence), nullptr);

		_vulkan_fence = nullptr;
	}
}
