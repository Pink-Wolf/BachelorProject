#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER

#include <cstdint>

namespace CompWolf::Graphics::Private
{
	/* Contains a 64-bit unsigned integer, but is its own unique class. */
	template <auto TypeKey>
	struct vulkan_handle64
	{
		uint64_t value;

		constexpr inline vulkan_handle64() noexcept = default;
		constexpr inline explicit vulkan_handle64(uint64_t value) noexcept : value(value) {}
		constexpr inline vulkan_handle64(std::nullptr_t) noexcept : value(0) {}

		constexpr inline explicit operator bool() { return value != 0; }
	};

	struct vulkan_instance_t {}; using vulkan_instance = vulkan_instance_t*;
	struct vulkan_debug_messenger_t {}; using vulkan_debug_messenger = vulkan_debug_messenger_t*;
	struct vulkan_physical_device_t {}; using vulkan_physical_device = vulkan_physical_device_t*;
	struct vulkan_device_t {}; using vulkan_device = vulkan_device_t*;

	struct glfw_window_t {}; using glfw_window = glfw_window_t*;

	// The following types are 64-bit values, even if pointers are only 32 bit long.
	// In Vulkan, these are non-dispatchable handles.

	using vulkan_surface = vulkan_handle64<0>;
	using vulkan_swapchain = vulkan_handle64<1>;
	using vulkan_image_view = vulkan_handle64<2>;
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
