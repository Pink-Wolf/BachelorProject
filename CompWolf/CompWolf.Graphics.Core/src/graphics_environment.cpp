#include "pch.h"
#include "graphics_environment.h"

#include "compwolf_vulkan.h"
#include <stdexcept>

namespace CompWolf::Graphics
{
	std::thread::id graphics_environment::main_graphics_thread;
	size_t graphics_environment::environment_counter(0);

	graphics_environment::graphics_environment()
	{
		if (environment_counter == 0)
		{
			{
				auto result = glfwInit();
				if (result != GLFW_TRUE)
				{
					std::runtime_error e("Could not set up glfw, used by graphics_environment");
					throw e;
				}
			}

			main_graphics_thread = std::this_thread::get_id();
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

	void graphics_environment::update()
	{
		graphics_environment_update_parameter args;
		updating(args);
		glfwPollEvents();
		updated(args);
	}
}
