#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"

namespace CompWolf::Graphics
{
	void Private::base_draw_command::compile(const gpu_command_compile_settings& settings)
	{
		auto command = Private::to_vulkan(settings.command);
		auto vkBuffer = Private::to_vulkan(base_buffer->vulkan_buffer());
		static VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(command, 0, 1, &vkBuffer, offsets);
		vkCmdDraw(command, static_cast<uint32_t>(base_buffer->size()), 1, 0, 0);
	}
}
