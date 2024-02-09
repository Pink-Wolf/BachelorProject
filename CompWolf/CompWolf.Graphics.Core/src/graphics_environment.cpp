#include "pch.h"
#include "graphics_environment.h"

#include "glfw.h"
#include <stdexcept>

namespace CompWolf::Graphics::Core
{
	size_t graphics_environment::environment_counter = 0;

	graphics_environment::graphics_environment()
	{
		if (environment_counter <= 0)
		{
			{
				auto result = glfwInit();
				if (result != GLFW_TRUE)
				{
					std::runtime_error e("Could not set up glfw, used by graphics_environment");
					throw e;
				}
			}
		}

		++environment_counter;
	}
	graphics_environment::~graphics_environment()
	{
		--environment_counter;

		if (environment_counter <= 0)
		{
			glfwTerminate();
		}
	}
}
