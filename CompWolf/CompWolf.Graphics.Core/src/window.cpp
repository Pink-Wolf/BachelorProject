#include "pch.h"
#include "window.h"

#include "compwolf_vulkan.h"

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
		glfw_window = other.glfw_window;
		other.glfw_window = nullptr;
	}
	auto window::operator=(window&& other) noexcept -> window&
	{
		glfw_window = other.glfw_window;
		other.glfw_window = nullptr;
		return *this;
	}
	window::~window()
	{
		close();
	}

	void window::close() noexcept
	{
		if (glfw_window != nullptr)
		{
			window_close_parameter args;
			closing(args);

			auto glfwWindow = Private::to_glfw(glfw_window);
			glfwDestroyWindow(glfwWindow);
			glfw_window = nullptr;

			closed(args);
		}
	}
}
