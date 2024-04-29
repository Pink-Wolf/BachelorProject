#ifndef COMPWOLF_GRAPHICS_GPU_VKBUFFER_ALLOCATOR_HEADER
#define COMPWOLF_GRAPHICS_GPU_VKBUFFER_ALLOCATOR_HEADER

#include "vulkan_types"
#include "base_gpu_buffer_allocator.hpp"
#include "gpu_vkBuffer_type.hpp"
#include "base_gpu_buffer.hpp"
#include <utility>
#include <initializer_list>

namespace CompWolf::Graphics
{
	class gpu_vkBuffer_allocator : public base_gpu_buffer_allocator
	{
	private: // fields
		std::size_t _size;
		std::size_t _element_stride;
		gpu_vkBuffer_type _type;

	public: // accessors
		/* Returns the amount of elements that the allocator allocates with alloc_data. */
		inline auto size() const noexcept { return _size; }
		/* Returns the byte-size of each element that the allocator allocates with alloc_data. */
		inline auto element_stride() const noexcept { return _element_stride; }
		/* Returns the gpu_vkBuffer_type that this allocator is. */
		inline auto type() const noexcept { return _type; }

	public: // pure functions
		/* Converts a vulkan_buffer into a data_handle. */
		auto from_buffer(Private::vulkan_buffer) const noexcept -> data_handle;
		/* Converts a data_handle into a vulkan_buffer. */
		auto to_buffer(data_handle) const noexcept -> Private::vulkan_buffer;

	public: // constructors
		gpu_vkBuffer_allocator() = default;
		gpu_vkBuffer_allocator(const gpu_vkBuffer_allocator&) = default;
		auto operator=(const gpu_vkBuffer_allocator&) -> gpu_vkBuffer_allocator& = default;
		gpu_vkBuffer_allocator(gpu_vkBuffer_allocator&&) = default;
		auto operator=(gpu_vkBuffer_allocator&&) -> gpu_vkBuffer_allocator& = default;

		gpu_vkBuffer_allocator(gpu_connection& gpu, gpu_vkBuffer_type type, std::size_t size, std::size_t element_stride)
			: base_gpu_buffer_allocator(gpu)
			, _type(type), _size(size), _element_stride(element_stride)
		{}

	public: // CompWolf::base_gpu_buffer_allocator
		auto alloc_data() const -> data_handle final;
		auto alloc_cpu_access(data_handle) const -> data_handle final;
		void bind_data(data_handle, Private::vulkan_memory) const final;
		void free_cpu_access(data_handle) const noexcept final;
		void free_data(data_handle) const noexcept final;
		void private_info(data_handle data, Private::gpu_buffer_private_info* out) const final;
	};

	namespace Private
	{
		struct gpu_vkBuffer_allocator_container
		{
			gpu_vkBuffer_allocator _allocator;
		};

		class base_gpu_vkBuffer_buffer : private gpu_vkBuffer_allocator_container, public base_gpu_buffer
		{
		public: // vulkan-related
			/* Returns the allocator used to allocate the buffer. */
			inline auto& vulkan_allocator() noexcept { return gpu_vkBuffer_allocator_container::_allocator; }
			/* Returns the allocator used to allocate the buffer. */
			inline auto& vulkan_allocator() const noexcept { return gpu_vkBuffer_allocator_container::_allocator; }

			/* Returns the buffer's vulkan_buffer, representing a VkBuffer. */
			inline auto vulkan_buffer() const { return vulkan_allocator().to_buffer(vulkan_data()); }

		public: // constructors
			base_gpu_vkBuffer_buffer() = default;
			base_gpu_vkBuffer_buffer(base_gpu_vkBuffer_buffer&&) = default;
			auto operator=(base_gpu_vkBuffer_buffer&&) -> base_gpu_vkBuffer_buffer& = default;
			inline ~base_gpu_vkBuffer_buffer() noexcept { free(); }

			/* Constructs a base_gpu_vkBuffer_buffer on the given gpu, with the given amount of elements.
			 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
			 */
			inline base_gpu_vkBuffer_buffer(gpu_connection& gpu, gpu_vkBuffer_type type, std::size_t size, std::size_t element_stride)
				: gpu_vkBuffer_allocator_container{ gpu_vkBuffer_allocator(gpu, type, size, element_stride) }
				, base_gpu_buffer(&vulkan_allocator())
			{}
			/* Constructs a base_gpu_vkBuffer_buffer on the given gpu, with the given data.
			 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
			 */
			template <typename ValueType>
			inline base_gpu_vkBuffer_buffer(gpu_connection& gpu, gpu_vkBuffer_type type, std::initializer_list<ValueType> data
			)
				: gpu_vkBuffer_allocator_container{ gpu_vkBuffer_allocator(gpu, type, data.size(), sizeof(ValueType))}
				, base_gpu_buffer(&vulkan_allocator(), std::move(data))
			{}

		public: // CompWolf::base_gpu_buffer
			void bind_to_shader(gpu_memory_bind_data*) const final;
		};
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_VKBUFFER_ALLOCATOR_HEADER
