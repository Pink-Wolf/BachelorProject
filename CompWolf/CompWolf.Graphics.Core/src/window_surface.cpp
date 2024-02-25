#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>

namespace CompWolf::Graphics
{
	window_surface::window_surface(graphics_environment& environment_input, Private::glfw_window& window) : _environment(&environment_input)
	{
		auto instance = Private::to_vulkan(_environment->get_vulkan_instance());

		try
		{
			{
				auto glfwWindow = Private::to_glfw(window);

				VkSurfaceKHR surface;
				auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not get a window's surface; unknown error from glfw/vulkan-function.");
				}

				_vulkan_surface = Private::from_vulkan(surface);
			}

			{
				auto glfwWindow = Private::to_glfw(window);
				auto surface = Private::to_vulkan(_vulkan_surface);

				std::unordered_map<Private::vulkan_physical_device, Private::present_device_info> device_infos;
				_draw_present_job = _environment->gpus().new_persistent_job(gpu_job_settings{
					.type = {gpu_job_type::present, gpu_job_type::draw},
					.priority = gpu_job_priority::high,
					.gpu_scorer = Private::evaluate_gpu_for_present(device_infos, _vulkan_surface),
					});
				auto& family = _environment->gpus().thread_family_for_job(_draw_present_job);
				auto& device_info = device_infos[family.vulkan_physical_device];

				VkExtent2D surface_extent;
				{
					int width, height;
					glfwGetFramebufferSize(glfwWindow, &width, &height);
					surface_extent = {
						.width = std::clamp<uint32_t>(width, device_info.capabilities.minImageExtent.width, device_info.capabilities.maxImageExtent.width),
						.height = static_cast<uint32_t>(height, device_info.capabilities.minImageExtent.height, device_info.capabilities.maxImageExtent.height),
					};
				}
				uint32_t swapchain_image_count = device_info.present_mode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;
				{
					auto min = device_info.capabilities.minImageCount;
					auto max = device_info.capabilities.maxImageCount; if (max == 0) max = swapchain_image_count;
					swapchain_image_count = std::clamp<uint32_t>(swapchain_image_count, min, max);
				}

				VkSwapchainCreateInfoKHR create_info{
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.surface = surface,
					.minImageCount = swapchain_image_count,
					.imageFormat = device_info.format.format,
					.imageColorSpace = device_info.format.colorSpace,
					.imageExtent = surface_extent,
					.imageArrayLayers = 1,
					.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.preTransform = device_info.capabilities.currentTransform,
					.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					.presentMode = device_info.present_mode,
					.clipped = VK_TRUE,
					.oldSwapchain = VK_NULL_HANDLE,
				};

				VkSwapchainKHR swapchain;
				auto result = vkCreateSwapchainKHR(Private::to_vulkan(family.vulkan_device), &create_info, nullptr, &swapchain);

				switch (result)
				{
				case VK_SUCCESS: break;
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: throw std::runtime_error("Could not create the swapchain for a window; something is already displaying images on the window.");
				default: throw std::runtime_error("Could not create swapchain for a window; unknown error from vulkan-function.");
				}

				_swapchain = Private::from_vulkan(swapchain);
			}
		}
		catch (...)
		{
			destroy(); // Make sure to release data

			throw;
		}

	}
	window_surface::window_surface(window_surface&& other) noexcept : _environment(other._environment)
	{
		_environment = std::move(other._environment);
		_vulkan_surface = std::move(other._vulkan_surface);
		_swapchain = std::move(other._swapchain);
		_draw_present_job = std::move(other._draw_present_job);

		other._vulkan_surface = nullptr;
	}
	auto window_surface::operator=(window_surface&& other) noexcept -> window_surface&
	{
		_environment = std::move(other._environment);
		_vulkan_surface = std::move(other._vulkan_surface);
		_swapchain = std::move(other._swapchain);
		_draw_present_job = std::move(other._draw_present_job);

		other._vulkan_surface = nullptr;

		return *this;
	}
	window_surface::~window_surface()
	{
		destroy();
	}

	void window_surface::destroy() noexcept
	{
		if (!_vulkan_surface) return;

		auto instance = Private::to_vulkan(_environment->get_vulkan_instance());
		auto vulkan_device = Private::to_vulkan(_environment->gpus().thread_family_for_job(_draw_present_job).vulkan_device);

		if (_swapchain) vkDestroySwapchainKHR(vulkan_device, Private::to_vulkan(_swapchain), nullptr);
		if (_vulkan_surface) vkDestroySurfaceKHR(instance, Private::to_vulkan(_vulkan_surface), nullptr);
		_vulkan_surface = nullptr;
	}
}
