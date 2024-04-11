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

	std::function<std::optional<float>(const gpu_connection&)> evaluate_gpu_for_present(std::unordered_map<Private::vulkan_physical_device, surface_format_info>& out_device_info, Private::vulkan_surface vulkan_surface, const window_surface_settings& settings);
	std::optional<surface_format_info> get_present_device_info(const gpu_connection& device, VkSurfaceKHR surface);
	std::optional<float> get_surface_format_score(const VkSurfaceFormatKHR& surface_format);
}

#endif // ! COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER
