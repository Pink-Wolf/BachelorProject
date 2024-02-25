#ifndef COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER
#define COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER

#include "compwolf_vulkan.hpp"
#include "private/graphics/graphics_environment.hpp"
#include <optional>

namespace CompWolf::Graphics::Private
{
	struct present_device_info
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR present_mode;
	};

	std::function<std::optional<float>(const gpu&)> evaluate_gpu_for_present(std::unordered_map<Private::vulkan_physical_device, present_device_info>& out_device_info, Private::vulkan_surface vulkan_surface);
	std::optional<present_device_info> get_present_device_info(const gpu& device, VkSurfaceKHR surface);
	std::optional<float> get_surface_format_score(const VkSurfaceFormatKHR& surface_format);
}

#endif // ! COMPWOLF_GRAPHICS_PRESENT_DEVICE_INFO_HEADER
