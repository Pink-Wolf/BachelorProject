#ifndef COMPWOLF_GRAPHICS_GPU_IMAGE_HEADER
#define COMPWOLF_GRAPHICS_GPU_IMAGE_HEADER

#include "vulkan_types"
#include "gpu_memory.hpp"
#include "shaders"
#include "stdexcept"
#include <utility>

namespace CompWolf::Graphics
{
	namespace Private
	{
		class gpu_image_allocator : public gpu_memory_allocator
		{
		public: // fields
			using value_type = transparent_color;
			int width;
			int height;
		public: // other methods
			auto from_image(Private::vulkan_image) const noexcept -> data_handle;
			auto to_image(data_handle) const noexcept -> Private::vulkan_image;
		public: // constructors
			gpu_image_allocator() = default;
			gpu_image_allocator(const gpu_image_allocator&) = default;
			auto operator=(const gpu_image_allocator&) -> gpu_image_allocator& = default;
			gpu_image_allocator(gpu_image_allocator&&) = default;
			auto operator=(gpu_image_allocator&&) -> gpu_image_allocator& = default;

			gpu_image_allocator(gpu_connection& device, int width, int height)
				: gpu_memory_allocator(device), width(width), height(height) {}
		public: // gpu_buffer_allocator
			auto alloc_data() const -> data_handle final;
			void bind_data(data_handle, vulkan_memory) const final;
			void free_data(data_handle) const noexcept final;
			void private_info(data_handle data, private_info_handle out) const final;
		};
	}

	class gpu_image_buffer : public basic_freeable
	{
	private: // fields
		using allocator_type = Private::gpu_image_allocator;
	public:
		using value_type = shader_image;
		using item_type = allocator_type::value_type;
	private:
		allocator_type _allocator;
		gpu_memory _memory;

		Private::vulkan_image_view _image_view;
		Private::vulkan_sampler _sampler;

	public: // getters
		inline auto& memory() noexcept { return _memory; }
		inline auto& memory() const noexcept { return _memory; }

		inline auto& device() noexcept { return memory().device(); }
		inline auto& device() const noexcept { return memory().device(); }

		inline auto& allocator() noexcept { return _allocator; }
		inline auto& allocator() const noexcept { return _allocator; }

		auto width() const { return allocator().width; }
		auto height() const { return allocator().height; }

		inline auto vulkan_image() const noexcept { return allocator().to_image(memory().vulkan_data()); }
		inline auto vulkan_image_view() const noexcept { return _image_view; }
		inline auto vulkan_sampler() const noexcept { return _sampler; }

	public: // other methods
		void bind_to_shader(gpu_memory::bind_handle) const;
		inline auto data() { return gpu_memory_access<item_type>(memory()); }

	private: // constructors
		void setup();
	public:
		gpu_image_buffer() = default;
		gpu_image_buffer(gpu_image_buffer&&) = default;
		auto operator=(gpu_image_buffer&&) -> gpu_image_buffer& = default;
		~gpu_image_buffer() noexcept { free(); }

		inline gpu_image_buffer(gpu_connection& target_device, int width, int height)
			: _allocator(target_device, width, height)
			, _memory(&_allocator, std::bind_front(&gpu_image_buffer::bind_to_shader, this))
		{
			setup();
		}

		inline gpu_image_buffer(gpu_connection& target_device, std::initializer_list<std::initializer_list<item_type>> pixels)
			: gpu_image_buffer(target_device
				, static_cast<int>(pixels.size())
				, (pixels.size() == 0) ? 0 : static_cast<int>(pixels.begin()->size())
			)
		{
			auto destination = data();
			
			auto source_line = pixels.begin();
			for (int y = 0; y < height() && source_line != pixels.end(); ++y, ++source_line)
			{
				auto source_pixel = source_line->begin();
				for (int x = 0; x < width() && source_pixel != source_line->end(); ++x, ++source_pixel)
				{
					new (&destination[y * width() + x]) (item_type) (std::move(*source_pixel));
				}
			}
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return memory().empty();
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_IMAGE_HEADER
