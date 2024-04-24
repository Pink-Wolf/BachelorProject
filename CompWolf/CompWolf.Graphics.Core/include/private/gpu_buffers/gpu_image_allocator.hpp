#ifndef COMPWOLF_GRAPHICS_GPU_IMAGE_ALLOCATOR_HEADER
#define COMPWOLF_GRAPHICS_GPU_IMAGE_ALLOCATOR_HEADER

#include "vulkan_types"
#include "base_gpu_buffer_allocator.hpp"
#include "base_gpu_buffer.hpp"
#include <utility>
#include <stdexcept>

namespace CompWolf::Graphics
{
	struct transparent_color;

	/* A base_gpu_buffer_allocator which allocates a vulkan_image, representing a VkImage. */
	class gpu_image_allocator : public base_gpu_buffer_allocator
	{
	public: // type definitions
		/* The type of pixels making up the image that the allocator allocates with alloc_data. */
		using value_type = transparent_color;

	private: // fields
		int _width;
		int _height;

	public: // accessors
		/* Returns the width of the image that the allocator allocates with alloc_data. */
		inline auto width() const noexcept { return _width; }
		/* Returns the height of the image that the allocator allocates with alloc_data. */
		inline auto height() const noexcept { return _height; }

	public: // pure functions
		auto from_image(Private::vulkan_image) const noexcept -> data_handle;
		auto to_image(data_handle) const noexcept -> Private::vulkan_image;

	public: // constructors
		/* Constructs a freed gpu_image_allocator, as in one that cannot be used to allocate memory. */
		gpu_image_allocator() = default;
		gpu_image_allocator(const gpu_image_allocator&) = default;
		auto operator=(const gpu_image_allocator&) -> gpu_image_allocator& = default;
		gpu_image_allocator(gpu_image_allocator&&) = default;
		auto operator=(gpu_image_allocator&&) -> gpu_image_allocator& = default;

		/* Constructs a gpu_image_allocator that will allocate an image with the given width and height in pixels, on the given gpu.
		 * @throws Throws std::domain_error if the given width or height is non-positive.
		 */
		inline gpu_image_allocator(gpu_connection& gpu, int width, int height)
			: base_gpu_buffer_allocator(gpu), _width(width), _height(height)
		{
			if (width <= 0) throw std::domain_error("Tried creating an image with a non-positive width.");
			if (height <= 0) throw std::domain_error("Tried creating an image with a non-positive height.");
		}

	public: // CompWolf::base_gpu_buffer_allocator
		auto alloc_data() const->data_handle final;
		auto alloc_cpu_access(data_handle) const -> data_handle final;
		void bind_data(data_handle, Private::vulkan_memory) const final;
		void free_cpu_access(data_handle) const noexcept final;
		void free_data(data_handle) const noexcept final;
		void private_info(data_handle data, Private::gpu_buffer_private_info* out) const final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_IMAGE_ALLOCATOR_HEADER
