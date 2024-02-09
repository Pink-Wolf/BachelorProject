#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER

#include "private/vulkan_types.h"
#include <version_number.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace CompWolf::Graphics::Private
{
#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(library, library_type, compwolf_type)					\
	inline library_type to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }			\
	inline compwolf_type from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }		\
	
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(glfw, GLFWwindow*, glfw_window*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkInstance, vulkan_instance*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkDebugUtilsMessengerEXT, vulkan_debug_messenger*)

	inline uint32_t to_vulkan(version_number a) { return VK_MAKE_API_VERSION(0, a.major, a.minor, a.patch); }

	template <typename FunctionType>
	FunctionType create_vulkan_function(VkInstance instance, const char* function_name)
	{
		auto function = vkGetInstanceProcAddr(instance, function_name);
		return (FunctionType)function;
	}
#define COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, function_name)\
	auto function_name = ::CompWolf::Graphics::Private::create_vulkan_function<PFN_##function_name>(instance, #function_name)
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
