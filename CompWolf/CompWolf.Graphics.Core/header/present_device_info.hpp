#ifndef COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER
#define COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER

#include "compwolf_vulkan.hpp"
#include "graphics"
#include "window"
#include "private/graphics/graphics_environment.hpp"
#include <optional>

namespace CompWolf::Graphics::Private
{
	struct surface_format_info
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR present_mode;
	};
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_POINTER_CONVERTERS(private, surface_format_info*, surface_format_handle_t*)

	gpu_connection* find_gpu_for_present(Private::vulkan_surface vulkan_surface, window_surface_settings& settings, surface_format_info* out_info) noexcept;
	std::optional<surface_format_info> get_present_device_info(const gpu_connection& device, VkSurfaceKHR surface);
	std::optional<float> get_surface_format_score(const VkSurfaceFormatKHR& surface_format);
}

#endif // ! COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER
