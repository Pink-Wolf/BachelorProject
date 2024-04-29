#ifndef COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_HEADER

#include "vulkan_types"
#include "base_gpu_buffer_allocator.hpp"
#include <freeable>
#include <owned>
#include <initializer_list>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct gpu_memory_bind_data;
	}
	class gpu_connection;

	class base_gpu_buffer : public basic_freeable
	{
	private: // fields
		owned_ptr<base_gpu_buffer_allocator*> _allocator;

		std::size_t _item_count;
		std::size_t _memory_size;

		base_gpu_buffer_allocator::data_handle _vulkan_data;
		Private::vulkan_memory _vulkan_memory;

	public: // accessors
		/* Returns the gpu that the buffer is on. */
		inline auto& gpu() noexcept { return _allocator->gpu(); }
		/* Returns the gpu that the buffer is on. */
		inline auto& gpu() const noexcept { return _allocator->gpu(); }

		/* Returns the amount of elements in the buffer. */
		inline auto size() const noexcept { return _item_count; }

	public: // vulkan-related
		/* Returns the allocator used to allocate the buffer. */
		inline auto vulkan_allocator() noexcept { return _allocator; }
		/* Returns the allocator used to allocate the buffer. */
		inline auto vulkan_allocator() const noexcept { return _allocator; }

		/* Returns the buffer's vulkan_memory, representing a VkDeviceMemory. */
		inline auto vulkan_memory() const noexcept { return _vulkan_memory; }
		/* Returns the size of the vulkan_memory. */
		inline auto vulkan_memory_size() const noexcept { return _memory_size; }

		/* Returns the data_handle received from the allocator's alloc_data-method. */
		inline auto vulkan_data() const { return _vulkan_data; }

		/* Binds the buffer to the given shader; as in sets the buffer as one used by the shader.
		 * The specific usage is specified by the given handle.
		 * @throws std::invalid_argument if the given shader is not on the same gpu as the buffer.
		 * @throws std::runtime_error if there was an error binding the data to the shader due to causes outside of the program.
		 */
		virtual void bind_to_shader(Private::gpu_memory_bind_data*) const = 0;

	public: // constructor
		base_gpu_buffer() = default;
		base_gpu_buffer(base_gpu_buffer&&) = default;
		auto operator=(base_gpu_buffer&&) -> base_gpu_buffer& = default;
		virtual inline ~base_gpu_buffer() noexcept { free(); }

		base_gpu_buffer(base_gpu_buffer_allocator*);
		template <typename ValueType>
		base_gpu_buffer(base_gpu_buffer_allocator*, std::initializer_list<ValueType>);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _allocator->empty();
		}
		void free() noexcept;
	};

	namespace Private
	{
		/* A smart-pointer to a base_gpu_buffer's memory.
		 * That is, one that on construction gets cpu-access to the buffer's data, and on destruction/freeing releases the access.
		 */
		class base_gpu_buffer_access : public basic_freeable
		{
		private: // fields
			owned_ptr<base_gpu_buffer*> _buffer;
			void* _data;

		public: // accessors
			/* Returns the buffer whose data this accesses. */
			inline auto& target_buffer() noexcept { return *_buffer; }
			/* Returns the buffer whose data this accesses. */
			inline auto& target_buffer() const noexcept { return *_buffer; }

			/* Returns a pointer to the start of the data this accesses. */
			inline auto target_buffer_data() noexcept { return _data; }
			/* Returns a pointer to the start of the data this accesses. */
			inline auto target_buffer_data() const noexcept { return _data; }

		public: // constructor
			base_gpu_buffer_access() = default;
			base_gpu_buffer_access(base_gpu_buffer_access&&) = default;
			auto operator=(base_gpu_buffer_access&&) -> base_gpu_buffer_access& = default;
			inline ~base_gpu_buffer_access() noexcept { free(); }

			/* Constructs a gpu_buffer_access that gives access to the given buffer's data.
		 * @throws std::invalid_argument if the given buffer is a nullptr.
			 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
			 */
			explicit base_gpu_buffer_access(base_gpu_buffer*);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_buffer;
			}
			void free() noexcept final;
		};
	}

	template <typename ValueType>
	base_gpu_buffer::base_gpu_buffer(base_gpu_buffer_allocator* allocator, std::initializer_list<ValueType> input_data)
		: base_gpu_buffer(allocator)
	{
		Private::base_gpu_buffer_access accessor(this);
		auto destination = static_cast<ValueType*>(accessor.target_buffer_data());
		auto source = input_data.begin();
		auto size = this->size() < input_data.size() ? this->size() : input_data.size();
		for (std::size_t i = 0; i < size; ++i, ++destination, ++source)
		{
			new (destination) (ValueType) (std::move(*source));
		}
	}
}

#endif // ! COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_HEADER
