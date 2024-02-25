#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>

namespace CompWolf::Graphics
{
	window::window(graphics_environment& environment_input) : _environment(&environment_input)
	{
		auto instance = Private::to_vulkan(_environment->get_vulkan_instance());

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

			_surface = window_surface(*_environment, _glfw_window.value());
		}
		catch (...)
		{
			close(); // Make sure to release data

			throw;
		}

	}
	window::window(window&& other) noexcept : _environment(other._environment)
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		_environment = std::move(other._environment);

		_glfw_window.value() = std::move(other._glfw_window.value());
		_surface = std::move(other._surface);

		other._glfw_window.value() = nullptr;
	}
	auto window::operator=(window&& other) noexcept -> window&
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		_environment = std::move(other._environment);

		_glfw_window.value() = std::move(other._glfw_window.value());
		_surface = std::move(other._surface);

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

		auto instance = Private::to_vulkan(_environment->get_vulkan_instance());

		{
			unique_value_lock<glfw_window_type> lock(_glfw_window);
			if (_glfw_window.value() == nullptr) return;

			_surface.destroy();

			auto glfwWindow = Private::to_glfw(_glfw_window.value());
			glfwDestroyWindow(glfwWindow);
			_glfw_window.value() = nullptr;
		}

		closed(args);
	}
}
