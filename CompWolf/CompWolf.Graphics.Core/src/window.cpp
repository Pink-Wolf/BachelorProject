#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <mutex>

namespace CompWolf::Graphics
{
	window::window(const graphics_environment& environment)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		auto glfwWindow = glfwCreateWindow(640, 480, "Window", nullptr, nullptr);
		_glfw_window = Private::from_glfw(glfwWindow);

		glfwSetWindowUserPointer(glfwWindow, this);

		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow) {
			window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
			this_window->close();
			});
	}
	window::window(window&& other) noexcept
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		_glfw_window.value() = std::move(other._glfw_window.value());
		other._glfw_window.value() = nullptr;
	}
	auto window::operator=(window&& other) noexcept -> window&
	{
		std::scoped_lock lock(_glfw_window, other._glfw_window);

		_glfw_window.value() = std::move(other._glfw_window.value());
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

		{
			unique_value_lock<glfw_window_type> lock(_glfw_window);
			if (_glfw_window.value() == nullptr) return;

			auto glfwWindow = Private::to_glfw(_glfw_window.value());
			glfwDestroyWindow(glfwWindow);
			_glfw_window.value() = nullptr;
		}

		closed(args);
	}
}
