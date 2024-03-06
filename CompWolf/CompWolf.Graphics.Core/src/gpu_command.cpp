#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"

namespace CompWolf::Graphics
{
	void draw_command::compile(Private::vulkan_command vulkan_command)
	{
		auto command = Private::to_vulkan(vulkan_command);
		vkCmdDraw(command, 3, 1, 0, 0);
	}
}
