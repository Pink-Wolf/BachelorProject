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
		glfw_window = Private::from_glfw(glfwWindow);

		glfwSetWindowUserPointer(glfwWindow, this);

		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow) {
			window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
			this_window->close();
			});
	}
	window::window(window&& other) noexcept
	{
		std::scoped_lock lock(glfw_window, other.glfw_window);

		glfw_window.value() = std::move(other.glfw_window.value());
		other.glfw_window.value() = nullptr;
	}
	auto window::operator=(window&& other) noexcept -> window&
	{
		std::scoped_lock lock(glfw_window, other.glfw_window);

		glfw_window.value() = std::move(other.glfw_window.value());
		other.glfw_window.value() = nullptr;

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
			shared_value_lock<glfw_window_type> lock(glfw_window);
			if (glfw_window.value() == nullptr) return;

			closing(args);
		}

		{
			unique_value_lock<glfw_window_type> lock(glfw_window);
			if (glfw_window.value() == nullptr) return;

			auto glfwWindow = Private::to_glfw(glfw_window.value());
			glfwDestroyWindow(glfwWindow);
			glfw_window.value() = nullptr;
		}

		closed(args);
	}
}
