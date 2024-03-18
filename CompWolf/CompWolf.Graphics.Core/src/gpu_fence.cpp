#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_fence::gpu_fence(const gpu& target_gpu, bool signaled)
	{
		_device = &target_gpu;
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		VkFenceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		};
		if (signaled) create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkFence fence;
		auto result = vkCreateFence(vulkan_device, &create_info, nullptr, &fence);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not create gpu fence.");
		}

		_vulkan_fence = Private::from_vulkan(fence);
	}

	bool gpu_fence::signaled() const
	{
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		return vkWaitForFences(vulkan_device, 1, &fence, VK_TRUE, 0) == VK_SUCCESS;
	}

	/******************************** other methods ********************************/

	void gpu_fence::wait() const
	{
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkWaitForFences(vulkan_device, 1, &fence, VK_TRUE, UINT64_MAX);
	}

	void gpu_fence::reset()
	{
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkResetFences(vulkan_device, 1, &fence);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_fence::free() noexcept
	{
		if (empty()) return;

		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		vkDestroyFence(vulkan_device, Private::to_vulkan(_vulkan_fence), nullptr);

		_vulkan_fence = nullptr;
	}
}
