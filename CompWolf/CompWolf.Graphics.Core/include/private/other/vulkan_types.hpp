#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER

#include <cstdint>

namespace CompWolf::Graphics::Private
{
	/* Contains a 64-bit unsigned integer, but is its own unique class.
	 * In Vulkan, these can be used to represent non-dispatchable handles
	 */
	template <auto TypeKey>
	struct vulkan_handle64
	{
		uint64_t value;

		constexpr inline vulkan_handle64() noexcept = default;
		constexpr inline explicit vulkan_handle64(uint64_t value) noexcept : value(value) {}
		constexpr inline vulkan_handle64(std::nullptr_t) noexcept : value(0) {}

		constexpr inline explicit operator bool() const { return value != 0; }
	};

	using vulkan_fence = vulkan_handle64<-1>;
	using vulkan_semaphore = vulkan_handle64<-2>;

	struct vulkan_instance_t {}; using vulkan_instance = vulkan_instance_t*;
	struct vulkan_debug_messenger_t {}; using vulkan_debug_messenger = vulkan_debug_messenger_t*;
	struct vulkan_physical_device_t {}; using vulkan_physical_device = vulkan_physical_device_t*;
	struct vulkan_device_t {}; using vulkan_device = vulkan_device_t*;
	struct vulkan_queue_t {}; using vulkan_queue = vulkan_queue_t*;

	struct glfw_window_t {}; using glfw_window = glfw_window_t*;
	using vulkan_surface = vulkan_handle64<0>;
	using vulkan_swapchain = vulkan_handle64<1>;
	using vulkan_image_view = vulkan_handle64<2>;
	struct surface_format_handle_t {}; using surface_format_handle = surface_format_handle_t*; using const_surface_format_handle = const surface_format_handle_t*;

	using vulkan_shader = vulkan_handle64<3>;
	using vulkan_pipeline_layout = vulkan_handle64<4>;
	using vulkan_render_pass = vulkan_handle64<5>;
	using vulkan_pipeline = vulkan_handle64<6>;
	using vulkan_frame_buffer = vulkan_handle64<7>;
	using vulkan_command_pool = vulkan_handle64<8>;
	struct vulkan_command_t {}; using vulkan_command = vulkan_command_t*;
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
