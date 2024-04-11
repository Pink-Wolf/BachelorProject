#include "pch.h"
#include "window"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	window_surface::window_surface(window_settings&, Private::glfw_window& window, window_surface_settings settings)
		: _vulkan_surface(nullptr), _format(nullptr), _render_pass(nullptr)
	{
		auto instance = Private::to_vulkan(settings.environment->vulkan_instance());
		auto glfwWindow = Private::to_glfw(window);

		try
		{
			VkSurfaceKHR surface;
			{
				auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not get a window's surface; unknown error from glfw/vulkan-function.");
				}

				_vulkan_surface = Private::from_vulkan(surface);
			}

			auto& surface_format = *new Private::surface_format_info();
			_format = Private::from_private(&surface_format);
			_target_gpu = Private::find_gpu_for_present(_vulkan_surface, settings, &surface_format);
			auto logicDevice = Private::to_vulkan(device().vulkan_device());

			VkRenderPass renderPass;
			{
				VkAttachmentDescription colorAttachment{
					.format = surface_format.format.format,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				};

				VkAttachmentReference colorAttachmentReference{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				};
				VkSubpassDescription subpass{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.colorAttachmentCount = 1,
					.pColorAttachments = &colorAttachmentReference,
				};

				VkRenderPassCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
					.attachmentCount = 1,
					.pAttachments = &colorAttachment,
					.subpassCount = 1,
					.pSubpasses = &subpass,
				};

				auto result = vkCreateRenderPass(logicDevice, &createInfo, nullptr, &renderPass);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"render pass\" for drawing on a window.");
				}

				_render_pass = Private::from_vulkan(renderPass);
			}
		}
		catch (...)
		{
			free(); // Make sure to release data

			throw;
		}

	}

	/******************************** CompWolf::freeable ********************************/

	void window_surface::free() noexcept
	{
		if (empty()) return;

		auto instance = Private::to_vulkan(device().vulkan_instance());
		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		if (_render_pass) vkDestroyRenderPass(logicDevice, Private::to_vulkan(_render_pass), nullptr);
		if (_format) delete Private::to_private(_format);
		if (_vulkan_surface) vkDestroySurfaceKHR(instance, Private::to_vulkan(_vulkan_surface), nullptr);

		_target_gpu = nullptr;
	}
}
