#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/* The amount of glfw_handles that the program currently has. */
	static size_t glfw_handle_counter;

	/******************************** constructors ********************************/

	glfw_handle::glfw_handle(const graphics_environment_settings&)
	{
		_not_empty = true;
		++glfw_handle_counter;
		if (glfw_handle_counter > 1) return;

		auto result = glfwInit();

		if (result != GLFW_TRUE)
		{
			throw std::runtime_error("Could not set up glfw, used by CompWolf::Graphics");
		}
	}

	glfw_handle::glfw_handle(glfw_handle&& other) noexcept
	{
		if (other.empty()) return;
		_not_empty = true;
		++glfw_handle_counter;
	}
	glfw_handle& glfw_handle::operator=(glfw_handle&& other) noexcept
	{
		if (other.empty()) return *this;
		_not_empty = true;
		++glfw_handle_counter;
		return *this;
	}

	/******************************** CompWolf::freeable ********************************/

	void glfw_handle::free() noexcept
	{
		if (empty()) return;

		--glfw_handle_counter;
		if (glfw_handle_counter > 0) return;

		glfwTerminate();

		_not_empty = false;
	}
}
