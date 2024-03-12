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

	window::window(graphics_environment& environment)
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
			}

			_surface = std::move(window_surface(environment, _glfw_window));
			_swapchain = std::move(window_swapchain(_glfw_window, _surface));
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

		for (auto& frame : swapchain().frames())
		{
			frame.drawing_fence.wait();
		}

		auto instance = Private::to_vulkan(device().vulkan_instance());
		window_close_parameter close_args;
		window_free_parameter free_args;

		closing(close_args);
		freeing(free_args);

		_swapchain.free();
		_surface.free();

		auto glfwWindow = Private::to_glfw(_glfw_window);
		glfwDestroyWindow(glfwWindow);
		_glfw_window = nullptr;

		closed(close_args);
		freed(free_args);
	}
}
