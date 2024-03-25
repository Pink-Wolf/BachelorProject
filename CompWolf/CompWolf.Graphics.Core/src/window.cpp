#include "pch.h"
#include "window"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>
#include <utility>
#include <compwolf_utility>

namespace CompWolf::Graphics
{
	/******************************** setters ********************************/

	void window::set_pixel_size(int width, int height)
	{
		window_rebuild_swapchain_parameter event_args{
			.old_surface = &_surface,
			.new_surface = nullptr,
			.old_swapchain = &_swapchain,
			.new_swapchain = nullptr,
		};
		swapchain_rebuilding(event_args);
		auto& old_device = device();

		_swapchain.free();
		_surface.free();

		_pixel_size.set(std::make_pair(width, height));

		_surface = window_surface(environment(), _glfw_window, window_surface_settings
			{
				.target_device = &old_device
			}
		);
		_swapchain = window_swapchain(_glfw_window, _surface);

		event_args = window_rebuild_swapchain_parameter{
			.old_surface = nullptr,
			.new_surface = &_surface,
			.old_swapchain = nullptr,
			.new_swapchain = &_swapchain,
		};
		swapchain_rebuilded(event_args);
	}

	/******************************** other methods ********************************/

	void window::update_image()
	{
		{
			auto& frame = swapchain().current_frame();
			auto semaphore = Private::to_vulkan(frame.pool.last_vulkan_semaphore());
			auto vkSwapchain = Private::to_vulkan(swapchain().vulkan_swapchain());
			auto frame_index = static_cast<uint32_t>(swapchain().current_frame_index());
			auto queue = Private::to_vulkan(draw_present_job().thread().queue);

			VkPresentInfoKHR presentInfo{
				.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = (semaphore == nullptr) ? 0_uint32 : 1_uint32,
				.pWaitSemaphores = &semaphore,
				.swapchainCount = 1,
				.pSwapchains = &vkSwapchain,
				.pImageIndices = &frame_index,
			};

			vkQueuePresentKHR(queue, &presentInfo);
		}

		for (auto& event : _draw_events)
		{
			event.second(*this);
		}
		_draw_events.clear();

		swapchain().to_next_frame();
	}

	/******************************** constructors ********************************/

	window::window(graphics_environment& environment) : _environment(&environment)
	{
		auto instance = Private::to_vulkan(environment.vulkan_instance());

		try
		{
			_pixel_size = std::make_pair(640, 480);

			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				auto glfwWindow = glfwCreateWindow(_pixel_size.value().first, _pixel_size.value().second, "Window", nullptr, nullptr);
				if (!glfwWindow) switch (glfwGetError(NULL))
				{
				case GLFW_API_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right API.");
				case GLFW_VERSION_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right version of OpenGL or openGL ES.");
				case GLFW_FORMAT_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right pixel format.");
				default: throw std::runtime_error("Could not create a window; unknown error from glfw-function.");
				}
				_glfw_window = Private::from_glfw(glfwWindow);

				glfwSetWindowUserPointer(glfwWindow, this);

				glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow) {
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->close();
					});

				glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->_draw_events.erase(draw_event_type::resize);
					this_window->_draw_events.emplace(draw_event_type::resize, [width, height](window& w)->void { w.set_pixel_size(width, height); });
					});
			}

			_surface = window_surface(environment, _glfw_window, window_surface_settings());
			_swapchain = window_swapchain(_glfw_window, _surface);
		}
		catch (...)
		{
			free(); // Make sure to release data

			throw;
		}

	}

	/******************************** CompWolf::freeable ********************************/

	void window::free() noexcept
	{
		if (empty()) return;

		window_close_parameter close_args;

		closing(close_args);
		freeing();

		_swapchain.free();
		_surface.free();

		auto glfwWindow = Private::to_glfw(_glfw_window);
		glfwDestroyWindow(glfwWindow);
		_glfw_window = nullptr;

		closed(close_args);
		freed();
	}
}
