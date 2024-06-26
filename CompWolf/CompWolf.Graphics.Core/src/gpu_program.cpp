#include "pch.h"
#include "gpu_programs"

#include "compwolf_vulkan.hpp"
#include <compwolf_utility>

namespace CompWolf
{
	/******************************** constructors ********************************/

	gpu_program::gpu_program(gpu_program_manager& manager, std::function<void(const gpu_program_code_parameters&)> code)
		: _manager(&manager)
		, _vulkan_command(nullptr)
	{
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

		VkCommandBuffer commandBuffer;
		{
			VkCommandBufferAllocateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = Private::to_vulkan(manager.vulkan_pool()),
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			auto result = vkAllocateCommandBuffers(logicDevice, &createInfo, &commandBuffer);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not set up \"command buffer\" for gpu commands.");
			}

			_vulkan_command = Private::from_vulkan(commandBuffer);
		}

		{
			{
				VkCommandBufferBeginInfo beginInfo{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				};

				auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not begin creating command for gpu commands.");
				}
			}

			gpu_program_code_parameters compile_parameter{
				.command = _vulkan_command
			};

			code(compile_parameter);

			{
				auto result = vkEndCommandBuffer(commandBuffer);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not create command for gpu commands.");
				}
			}
		}
	}

	/******************************** modifiers ********************************/

	auto gpu_program::execute() -> gpu_fence&
	{
		auto& gpu_device = gpu();

		auto& thread = manager().thread();
		auto queue = Private::to_vulkan(thread.queue);

		auto oldSemaphore = Private::to_vulkan(manager().last_vulkan_semaphore());
		auto& sync = manager().push_synchronization(gpu_program_sync
			{
				.fence = gpu_fence(gpu_device),
				.semaphore = gpu_semaphore(gpu_device),
			}
		);
		auto fence = Private::to_vulkan(sync.fence.vulkan_fence());
		auto semaphore = Private::to_vulkan(sync.semaphore.vulkan_semaphore());

		auto vulkanCommand = Private::to_vulkan(_vulkan_command);

		{
			VkSubmitInfo submitInfo{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = (oldSemaphore == nullptr) ? 0_uint32 : 1_uint32,
				.pWaitSemaphores = &oldSemaphore,
				.commandBufferCount = 1,
				.pCommandBuffers = &vulkanCommand,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &semaphore,
			};

			auto result = vkQueueSubmit(queue, 1, &submitInfo, fence);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not execute command; its commands could not be submitted to GPU.");
			}
		}

		return sync.fence;
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_program::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

		vkDeviceWaitIdle(logicDevice);

		auto command = Private::to_vulkan(_vulkan_command);
		vkFreeCommandBuffers(logicDevice, Private::to_vulkan(manager().vulkan_pool()), 1, &command);

		_manager = nullptr;
	}
}
