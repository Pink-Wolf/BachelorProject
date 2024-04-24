#ifndef COMPWOLF_GRAPHICS_GPU_IMAGE_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_IMAGE_BUFFER_HEADER

#include "vulkan_types"
#include "gpu_image_allocator.hpp"
#include "base_gpu_buffer.hpp"
#include "stdexcept"
#include <utility>
#include <initializer_list>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct gpu_image_allocator_container
		{
			gpu_image_allocator _allocator;
		};
	}
	struct shader_image;

	/* Contains field-data for one or more shaders, containing an image. */
	class gpu_image_buffer : private Private::gpu_image_allocator_container, public base_gpu_buffer
	{
	public: // type definitions
		/* The type of data in the buffer. */
		using value_type = shader_image;
		/* The type of pixels making up the image. */
		using pixel_type = gpu_image_allocator::value_type;

	private: // fields
		Private::vulkan_image_view _image_view;
		Private::vulkan_sampler _sampler;

	public: // vulkan-related
		/* Returns the allocator used to allocate the buffer. */
		inline auto& vulkan_allocator() noexcept { return gpu_image_allocator_container::_allocator; }
		/* Returns the allocator used to allocate the buffer. */
		inline auto& vulkan_allocator() const noexcept { return gpu_image_allocator_container::_allocator; }

		/* Returns the image's vulkan_image, representing a VkImage. */
		inline auto vulkan_image() const noexcept { return vulkan_allocator().to_image(vulkan_data()); }
		/* Returns the image's vulkan_image_view, representing a VkImageView. */
		inline auto vulkan_image_view() const noexcept { return _image_view; }

		/* Returns the image's vulkan_sampler, representing a VkSampler. */
		inline auto vulkan_sampler() const noexcept { return _sampler; }

	public: // accessors
		/* Returns the width of the image in the buffer. */
		inline auto width() const noexcept { return vulkan_allocator().width(); }
		/* Returns the height of the image in the buffer. */
		inline auto height() const noexcept { return vulkan_allocator().height(); }

		/* Returns a gpu_buffer_access to read and modify the buffer's data.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		inline auto data() { return gpu_buffer_access<pixel_type>(this); }

	private: // constructors
		void setup();
	public:
		gpu_image_buffer() = default;
		gpu_image_buffer(gpu_image_buffer&&) = default;
		auto operator=(gpu_image_buffer&&) -> gpu_image_buffer& = default;
		~gpu_image_buffer() noexcept { free(); }

		/* Constructs a gpu_image_buffer on the given gpu, with the given width and height in pixels.
		 * @throws std::domain_error if the given width or height is non-positive.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_image_buffer(gpu_connection& gpu, int width, int height)
			: gpu_image_allocator_container{ gpu_image_allocator(gpu, width, height) }
			, base_gpu_buffer(&vulkan_allocator())
		{
			setup();
		}
		/* Constructs a gpu_image_buffer on the given gpu, with the given width and height in pixels.
		 * @throws std::domain_error if the given width or height is non-positive.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		gpu_image_buffer(gpu_connection& gpu, std::initializer_list<std::initializer_list<pixel_type>> input)
			: gpu_image_buffer(gpu, static_cast<int>(input.size()), static_cast<int>(input.begin()->size()))
		{
			auto destination = data();

			auto source_line = input.begin();
			for (int y = 0; y < height(); ++y, ++source_line)
			{
				auto source_pixel = source_line->begin();
				auto width = this->width() < source_line->size() ? this->width() : source_line->size();
				for (int x = 0; x < width; ++x, ++source_pixel)
				{
					new (&destination[y * this->width() + x]) (pixel_type) (std::move(*source_pixel));
				}
			}
		}

	public: // CompWolf::base_gpu_buffer
		void bind_to_shader(Private::gpu_memory_bind_data*) const final;

	public: // CompWolf::freeable
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_IMAGE_BUFFER_HEADER
