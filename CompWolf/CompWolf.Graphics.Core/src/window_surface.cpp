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

	window_surface::window_surface(window_settings& settings
		, Private::glfw_window& window
		, graphics_environment* optional_environment
		, gpu_connection* optional_gpu)
		: _vulkan_surface(nullptr), _format(nullptr), _render_pass(nullptr)
	{
		auto vulkan_instance = optional_environment
			? optional_environment->vulkan_instance()
			: optional_gpu->vulkan_instance();
		auto instance = Private::to_vulkan(vulkan_instance);
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
			if (optional_gpu)
			{
				_target_gpu = optional_gpu;
				auto optional_surface = Private::get_present_device_info(*optional_gpu, surface);
				if (!optional_surface.has_value()) throw std::runtime_error("Could not create a window; its specified gpu cannot draw on the window.");
				surface_format = optional_surface.value();
			}
			else
			{
				_target_gpu = Private::find_gpu_for_present(_vulkan_surface, *optional_environment, &surface_format);
				if (!_target_gpu) throw std::runtime_error("Could not create a window; no suitable gpu.");
			}
			auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

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

		auto instance = Private::to_vulkan(gpu().vulkan_instance());
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

		if (_render_pass) vkDestroyRenderPass(logicDevice, Private::to_vulkan(_render_pass), nullptr);
		if (_format) delete Private::to_private(_format);
		if (_vulkan_surface) vkDestroySurfaceKHR(instance, Private::to_vulkan(_vulkan_surface), nullptr);

		_target_gpu = nullptr;
	}
}
