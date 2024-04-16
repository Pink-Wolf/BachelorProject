#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER

#include "vulkan_types"
#include "shaders"
#include "gpu_memory.hpp"
#include <utility>

namespace CompWolf::Graphics
{
	namespace Private
	{
		enum class gpu_buffer_type
		{
			index,
			vertex,
			uniform,
		};
		class gpu_buffer_allocator : public gpu_memory_allocator
		{
		public: // fields
			std::size_t size;
			std::size_t element_stride;
			gpu_buffer_type type;
		public: // other methods
			auto from_buffer(Private::vulkan_buffer) const noexcept -> data_handle;
			auto to_buffer(data_handle) const noexcept -> Private::vulkan_buffer;
		public: // constructors
			gpu_buffer_allocator() = default;
			gpu_buffer_allocator(const gpu_buffer_allocator&) = default;
			auto operator=(const gpu_buffer_allocator&) -> gpu_buffer_allocator& = default;
			gpu_buffer_allocator(gpu_buffer_allocator&&) = default;
			auto operator=(gpu_buffer_allocator&&) -> gpu_buffer_allocator& = default;

			gpu_buffer_allocator(gpu_connection& device, gpu_buffer_type type, std::size_t size, std::size_t element_stride)
				: gpu_memory_allocator(device), type(type), size(size), element_stride(element_stride) {}
		public: // gpu_buffer_allocator
			auto alloc_data() const -> data_handle final;
			void bind_data(data_handle, vulkan_memory) const final;
			void free_data(data_handle) const noexcept final;
			void private_info(data_handle data, private_info_handle out) const final;
		};

		class base_gpu_buffer : public basic_freeable
		{
		private: // fields
			gpu_buffer_allocator _allocator;
			gpu_memory _memory;

		public: // getter
			inline auto& memory() noexcept { return _memory; }
			inline auto& memory() const noexcept { return _memory; }

			inline auto& device() noexcept { return memory().device(); }
			inline auto& device() const noexcept { return memory().device(); }

			inline auto& allocator() noexcept { return _allocator; }
			inline auto& allocator() const noexcept { return _allocator; }

			auto size() const { return allocator().size; }

			inline auto vulkan_buffer() const noexcept { return _allocator.to_buffer(memory().vulkan_data()); }

		public: // other methods
			void bind_to_shader(gpu_memory::bind_handle) const;
			
		public: // constructor
			base_gpu_buffer() = default;
			base_gpu_buffer(base_gpu_buffer&&) = default;
			auto operator=(base_gpu_buffer&&) -> base_gpu_buffer& = default;
			~base_gpu_buffer() noexcept { free(); }

			base_gpu_buffer(gpu_connection& device, gpu_buffer_type type, std::size_t size, std::size_t element_stride)
				: _allocator(device, type, size, element_stride)
				, _memory(&_allocator, std::bind_front(&base_gpu_buffer::bind_to_shader, this))
			{}
			template <typename DataType>
			base_gpu_buffer(gpu_connection& device, gpu_buffer_type type, std::initializer_list<DataType> data)
				: _allocator(device, type, data.size(), sizeof(DataType))
				, _memory(&_allocator, std::bind_front(&base_gpu_buffer::bind_to_shader, this), data)
			{}

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return memory().empty();
			}
			inline void free() noexcept final { _memory.free(); }
		};
	}


	class gpu_index_buffer : public Private::base_gpu_buffer
	{
	public: // fields
		using value_type = shader_int;
	private:
		using super = Private::base_gpu_buffer;

	public: // other methods
		inline auto data() { return gpu_memory_access<value_type>(memory()); }

	public: // cosntructors
		gpu_index_buffer() = default;
		gpu_index_buffer(gpu_index_buffer&&) = default;
		auto operator=(gpu_index_buffer&&) -> gpu_index_buffer& = default;

		inline gpu_index_buffer(gpu_connection& target_device, std::size_t size)
			: super(target_device, Private::gpu_buffer_type::index, size, sizeof(shader_int))
		{}

		inline gpu_index_buffer(gpu_connection& target_device, std::initializer_list<shader_int> data)
			: super(target_device, Private::gpu_buffer_type::index, data)
		{}
	};
	template <typename VertexType>
	class gpu_vertex_buffer : public Private::base_gpu_buffer
	{
	public: // fields
		using value_type = VertexType;
	private:
		using super = Private::base_gpu_buffer;

	public: // other methods
		inline auto data() { return gpu_memory_access<value_type>(memory()); }

	public: // cosntructors
		gpu_vertex_buffer() = default;
		gpu_vertex_buffer(gpu_vertex_buffer&&) = default;
		auto operator=(gpu_vertex_buffer&&) -> gpu_vertex_buffer& = default;

		inline gpu_vertex_buffer(gpu_connection& target_device, std::size_t size)
			: super(target_device, Private::gpu_buffer_type::vertex, size, sizeof(VertexType))
		{}

		inline gpu_vertex_buffer(gpu_connection& target_device, std::initializer_list<VertexType> data)
			: super(target_device, Private::gpu_buffer_type::vertex, data)
		{}
	};
	template <typename DataType>
	class gpu_uniform_buffer : public Private::base_gpu_buffer
	{
	public: // fields
		using value_type = DataType;
	private:
		using super = Private::base_gpu_buffer;

	public: // other methods
		inline auto data() { return gpu_memory_access<value_type>(memory()); }
		inline auto single_data() { return gpu_single_memory_access<value_type>(memory()); }

	public: // cosntructors
		gpu_uniform_buffer() = default;
		gpu_uniform_buffer(gpu_uniform_buffer&&) = default;
		auto operator=(gpu_uniform_buffer&&) -> gpu_uniform_buffer& = default;

		inline gpu_uniform_buffer(gpu_connection& target_device, std::size_t size)
			: super(target_device, Private::gpu_buffer_type::uniform, size, sizeof(DataType))
		{}

		inline gpu_uniform_buffer(gpu_connection& target_device, std::initializer_list<DataType> data)
			: super(target_device, Private::gpu_buffer_type::uniform, data)
		{}
		inline gpu_uniform_buffer(gpu_connection& target_device, DataType data)
			: gpu_uniform_buffer(target_device, { data })
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
