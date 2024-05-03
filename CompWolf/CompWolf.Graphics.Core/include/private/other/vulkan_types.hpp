#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER

#include <cstdint>

namespace CompWolf::Private
{
	/* Contains a 64-bit unsigned integer, but is its own unique class.
	 * In Vulkan, these can be used to represent non-dispatchable handles
	 */
	template <typename TypeKey>
	struct vulkan_handle64
	{
		uint64_t value;

		constexpr inline operator uint64_t() { return value; }

		constexpr inline vulkan_handle64() noexcept = default;
		constexpr inline vulkan_handle64(uint64_t value) noexcept : value(value) {}
		constexpr inline vulkan_handle64(std::nullptr_t) noexcept : value(0) {}

		constexpr inline explicit operator bool() const { return value != 0; }
	};

	using vulkan_fence = vulkan_handle64<struct vulkan_fence_t>;
	using vulkan_semaphore = vulkan_handle64<struct vulkan_semaphore_t>;

	using vulkan_instance = struct vulkan_instance_t*;
	using vulkan_debug_messenger = struct vulkan_debug_messenger_t*;
	using vulkan_physical_device = struct vulkan_physical_device_t*;
	using vulkan_device = struct vulkan_device_t*;
	using vulkan_queue = struct vulkan_queue_t*;

	using glfw_window = struct glfw_window_t*;
	using vulkan_surface = vulkan_handle64<struct vulkan_surface_t>;
	using vulkan_swapchain = vulkan_handle64<struct vulkan_swapchain_t>;
	using surface_format_handle = struct surface_format_handle_t*;

	using vulkan_image_view = vulkan_handle64<struct vulkan_image_view_t>;
	using vulkan_sampler = vulkan_handle64<struct vulkan_sampler_t>;

	using shader_field_info_handle = struct shader_field_info_handle_t*;
	using vulkan_buffer = vulkan_handle64<struct vulkan_buffer_t>;
	using vulkan_memory = vulkan_handle64<struct vulkan_memory_t>;
	using vulkan_shader = vulkan_handle64<struct vulkan_shader_t>;
	using vulkan_pipeline_layout = vulkan_handle64<struct vulkan_pipeline_layout_t>;
	using vulkan_pipeline_layout_descriptor = vulkan_handle64<struct vulkan_pipeline_layout_descriptor_t>;
	using vulkan_descriptor_pool = vulkan_handle64<struct vulkan_descriptor_pool_t>;
	using vulkan_descriptor_set = vulkan_handle64<struct vulkan_descriptor_set_t>;
	using vulkan_render_pass = vulkan_handle64<struct vulkan_render_pass_t>;
	using vulkan_pipeline = vulkan_handle64<struct vulkan_pipeline_t>;
	using vulkan_frame_buffer = vulkan_handle64<struct vulkan_frame_buffer_t>;
	using vulkan_command_pool = vulkan_handle64<struct vulkan_command_pool_t>;
	struct vulkan_command_t {};							using vulkan_command = vulkan_command_t*;

	using vulkan_image = vulkan_handle64<struct vulkan_image_t>;
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_TYPES_HEADER
