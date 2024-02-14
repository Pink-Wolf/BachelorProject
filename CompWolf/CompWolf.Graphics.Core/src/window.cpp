#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <mutex>

namespace CompWolf::Graphics
{
	window::window(const graphics_environment& environment_input) : environment(&environment_input)
	{
		auto instance = Private::to_vulkan(environment->get_vulkan_instance());

		try
		{
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				auto glfwWindow = glfwCreateWindow(640, 480, "Window", nullptr, nullptr);
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

			{
				auto glfwWindow = Private::to_glfw(_glfw_window.value());

				VkSurfaceKHR surface;
				auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not get a window's surface; unknown error from glfw/vulkan-function.");
				}

				_vulkan_surface = Private::from_vulkan(surface);
			}
		}
		catch (...)
		{
			close(); // Make sure to release data

			throw;
		}

	}
	window::window(window&& other) noexcept : environment(other.environment)
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		_glfw_window.value() = std::move(other._glfw_window.value());
		_vulkan_surface = std::move(other._vulkan_surface);

		other._glfw_window.value() = nullptr;
	}
	auto window::operator=(window&& other) noexcept -> window&
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		environment = other.environment;

		_glfw_window.value() = std::move(other._glfw_window.value());
		_vulkan_surface = std::move(other._vulkan_surface);

		other._glfw_window.value() = nullptr;

		return *this;
	}
	window::~window()
	{
		close();
	}

	void window::close() noexcept
	{
		window_close_parameter args;

		{
			shared_value_lock<glfw_window_type> lock(_glfw_window);
			if (_glfw_window.value() == nullptr) return;

			closing(args);
		}

		auto instance = Private::to_vulkan(environment->get_vulkan_instance());

		{
			unique_value_lock<glfw_window_type> lock(_glfw_window);
			if (_glfw_window.value() == nullptr) return;

			if (_vulkan_surface) vkDestroySurfaceKHR(instance, Private::to_vulkan(_vulkan_surface), nullptr);

			auto glfwWindow = Private::to_glfw(_glfw_window.value());
			glfwDestroyWindow(glfwWindow);
			_glfw_window.value() = nullptr;
		}

		closed(args);
	}
}
