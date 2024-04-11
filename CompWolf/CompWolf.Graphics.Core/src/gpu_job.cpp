#include "pch.h"
#include "gpus"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_job::gpu_job(gpu_connection& device_in, std::size_t family_index_in, std::size_t thread_index_in)
		: _device(&device_in)
		, _family_index(family_index_in)
		, _thread_index(thread_index_in)
		, _vulkan_pool(nullptr)
	{
		try
		{
			++family().job_count;
			++thread().job_count;

			auto logicDevice = Private::to_vulkan(device().vulkan_device());

			VkCommandPool commandPool;
			{
				VkCommandPoolCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
					.queueFamilyIndex = static_cast<uint32_t>(family_index()),
				};

				auto result = vkCreateCommandPool(logicDevice, &createInfo, nullptr, &commandPool);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"command pool\", used to store gpu-programs on the gpu.");
				}

				_vulkan_pool = Private::from_vulkan(commandPool);
			}
		}
		catch (...)
		{
			free();
			throw;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_job::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		vkDeviceWaitIdle(logicDevice);
		vkDestroyCommandPool(logicDevice, Private::to_vulkan(_vulkan_pool), nullptr);

		--family().job_count;
		--thread().job_count;

		_device = nullptr;
	}
}
