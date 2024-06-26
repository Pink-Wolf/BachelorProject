#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <utility>

namespace CompWolf
{
	/* The amount of glfw_handles that the program currently has. */
	static std::size_t glfw_handle_counter;

	/******************************** constructors ********************************/

	glfw_handle::glfw_handle(const graphics_environment_settings&)
	{
		_not_empty = true;
		++glfw_handle_counter;
		if (glfw_handle_counter > 1) return;

		auto result = glfwInit();

		if (result != GLFW_TRUE)
		{
			throw std::runtime_error("Could not set up glfw, used by CompWolf");
		}
	}

	glfw_handle::glfw_handle(glfw_handle&& other) noexcept
	{
		if (other.empty()) return;
		_not_empty = true;
		other._not_empty = false;
	}
	glfw_handle& glfw_handle::operator=(glfw_handle&& other) noexcept
	{
		if (other.empty()) return *this;
		_not_empty = true;
		other._not_empty = false;
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
