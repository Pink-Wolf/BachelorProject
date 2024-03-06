#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace CompWolf::Graphics
{
	gpu_fence::gpu_fence(const gpu& target_gpu) : basic_const_gpu_user(target_gpu)
	{
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
	void gpu_fence::clear() noexcept
	{
		if (empty()) return;

		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		vkDestroyFence(vulkan_device, Private::to_vulkan(_vulkan_fence), nullptr);
	}

	gpu_fence::gpu_fence(gpu_fence&& other)
	{
		set_device(other.device());
		_vulkan_fence = std::move(other._vulkan_fence);

		other._vulkan_fence = nullptr;
	}
	auto gpu_fence::operator=(gpu_fence&& other) -> gpu_fence&
	{
		set_device(other.device());
		_vulkan_fence = std::move(other._vulkan_fence);

		other._vulkan_fence = nullptr;

		return *this;
	}

	void gpu_fence::wait() const
	{
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());
		auto fence = Private::to_vulkan(_vulkan_fence);

		vkWaitForFences(vulkan_device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	}
}
