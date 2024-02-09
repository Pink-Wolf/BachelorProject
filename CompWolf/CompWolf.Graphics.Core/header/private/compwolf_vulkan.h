#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER

#include "private/vulkan_types.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace CompWolf::Graphics::Private
{
	inline GLFWwindow* to_glfw(glfw_window* window) { return reinterpret_cast<GLFWwindow*>(window); }
	inline glfw_window* from_glfw(GLFWwindow* window) { return reinterpret_cast<glfw_window*>(window); }
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
