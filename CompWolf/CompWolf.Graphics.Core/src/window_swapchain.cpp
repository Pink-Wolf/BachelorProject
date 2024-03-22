#include "pch.h"
#include "window"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>

#include <iostream>

namespace CompWolf::Graphics
{
	/******************************** other methods ********************************/

	void window_swapchain::to_next_frame()
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto swapchain = Private::to_vulkan(vulkan_swapchain());

		uint32_t index;
		gpu_fence fence(device());
		auto result = vkAcquireNextImageKHR(logicDevice, swapchain, UINT64_MAX, VK_NULL_HANDLE, Private::to_vulkan(fence.vulkan_fence()), &index);

		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		default: throw std::runtime_error("Could not get next frame.");
		}

		_current_frame_index = static_cast<std::size_t>(index);

		fence.wait();
		vkDeviceWaitIdle(logicDevice); // this is done as above wait seems to not actually wait as it should; the following link reports the same problem: https://forums.developer.nvidia.com/t/problems-with-vk-khr-swapchain/43513
		current_frame().synchronizations.clear();
	}

	/******************************** constructors ********************************/

	window_swapchain::window_swapchain(Private::glfw_window window, window_surface& window_surface)
	{
		auto glfwWindow = Private::to_glfw(window);

		_target_gpu = &window_surface.device();
		auto instance = Private::to_vulkan(device().vulkan_instance());
		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		auto surface = Private::to_vulkan(window_surface.surface());
		auto& surface_format = *Private::to_private(window_surface.format());

		auto& job = window_surface.draw_present_job();

		try
		{
			// Swapchain
			VkSwapchainKHR swapchain;
			{
				VkExtent2D surfaceExtent;
				{
					int width, height;
					glfwGetFramebufferSize(glfwWindow, &width, &height);
					surfaceExtent = {
						.width = std::clamp<uint32_t>(width, surface_format.capabilities.minImageExtent.width, surface_format.capabilities.maxImageExtent.width),
						.height = static_cast<uint32_t>(height, surface_format.capabilities.minImageExtent.height, surface_format.capabilities.maxImageExtent.height),
					};
				}
				uint32_t swapchain_image_count = surface_format.present_mode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;
				{
					auto min = surface_format.capabilities.minImageCount;
					auto max = surface_format.capabilities.maxImageCount; if (max == 0) max = swapchain_image_count;
					swapchain_image_count = std::clamp<uint32_t>(swapchain_image_count, min, max);
				}

				VkSwapchainCreateInfoKHR createInfo{
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.surface = surface,
					.minImageCount = swapchain_image_count,
					.imageFormat = surface_format.format.format,
					.imageColorSpace = surface_format.format.colorSpace,
					.imageExtent = surfaceExtent,
					.imageArrayLayers = 1,
					.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.preTransform = surface_format.capabilities.currentTransform,
					.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					.presentMode = surface_format.present_mode,
					.clipped = VK_TRUE,
					.oldSwapchain = VK_NULL_HANDLE,
				};

				auto result = vkCreateSwapchainKHR(logicDevice, &createInfo, nullptr, &swapchain);

				switch (result)
				{
				case VK_SUCCESS: break;
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: throw std::runtime_error("Could not create the swapchain for a window; something is already displaying images on the window.");
				default: throw std::runtime_error("Could not create swapchain for a window; unknown error from vulkan-function.");
				}

				_vulkan_swapchain = Private::from_vulkan(swapchain);
			}

			// Swapchain images
			{
				auto images = Private::get_size_and_vector<uint32_t, VkImage>([logicDevice, swapchain](uint32_t* size, VkImage* data)
					{
						auto result = vkGetSwapchainImagesKHR(logicDevice, swapchain, size, data);
						switch (result)
						{
						case VK_SUCCESS:
						case VK_INCOMPLETE:
							break;
						default: throw std::runtime_error("Could not get a window's swapchain-images");
						}
					}
				);

				_frames.reserve(images.size());
				for (std::size_t i = 0; i < images.size(); ++i)
				{
					auto& image = images;

					VkImageViewCreateInfo createInfo{
						.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						.image = images[i],
						.viewType = VK_IMAGE_VIEW_TYPE_2D,
						.format = surface_format.format.format,
						.components{
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY,
						},
						.subresourceRange{
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1,
						},
					};

					VkImageView view;
					auto result = vkCreateImageView(logicDevice, &createInfo, nullptr, &view);

					switch (result)
					{
					case VK_SUCCESS: break;
					default: throw std::runtime_error("Could not get connection to a window's swapchain-images");
					}

					_frames.push_back(std::move(swapchain_frame
						{
							.image = Private::from_vulkan(view),
						}
					));
				}

				for (auto& frame : _frames)
				{
					VkCommandPoolCreateInfo createInfo{
						.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
						.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
						.queueFamilyIndex = static_cast<uint32_t>(device().index_of_family(job.family())),
					};

					VkCommandPool commandPool;
					auto result = vkCreateCommandPool(logicDevice, &createInfo, nullptr, &commandPool);

					switch (result)
					{
					case VK_SUCCESS: break;
					default: throw std::runtime_error("Could not set up \"command pool\"s for a window's swapchain-images.");
					}

					frame.command_pool = Private::from_vulkan(commandPool);
				}
			}

			to_next_frame();
		}
		catch (...)
		{
			free(); // Make sure to release data

			throw;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void window_swapchain::free() noexcept
	{
		if (empty()) return;

		auto instance = Private::to_vulkan(device().vulkan_instance());
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto vkSwapchain = Private::to_vulkan(_vulkan_swapchain);

		vkDeviceWaitIdle(logicDevice);

		for (auto& frame : _frames)
		{
			if (frame.image) vkDestroyImageView(logicDevice, Private::to_vulkan(frame.image), nullptr);
			if (frame.command_pool) vkDestroyCommandPool(logicDevice, Private::to_vulkan(frame.command_pool), nullptr);
		}
		_frames.clear();
		if (_vulkan_swapchain) vkDestroySwapchainKHR(logicDevice, Private::to_vulkan(_vulkan_swapchain), nullptr);

		_target_gpu = nullptr;
	}
}
