#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER

#include "private/vulkan_types.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace CompWolf::Graphics::Private
{
#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(library, library_type, compwolf_type)					\
	inline library_type to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }			\
	inline compwolf_type from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }		\
	
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(glfw, GLFWwindow*, glfw_window*);
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkInstance, vulkan_instance*);
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
