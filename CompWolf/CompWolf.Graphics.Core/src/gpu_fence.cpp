#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_fence::gpu_fence(const gpu& target_gpu)
	{
		_device = &target_gpu;
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		VkFenceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};

		VkFence fence;
		auto result = vkCreateFence(vulkan_device, &create_info, nullptr, &fence);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not create gpu fence.");
		}

		_vulkan_fence = Private::from_vulkan(fence);
	}

	/******************************** other methods ********************************/

	void gpu_fence::wait() const
	{
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkWaitForFences(vulkan_device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
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
