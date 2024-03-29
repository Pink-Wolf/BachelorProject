#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER

#include "vulkan_types"
#include "gpu"
#include "shader_field.hpp"
#include <freeable>
#include <span>
#include <initializer_list>
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

		class base_gpu_buffer : public basic_freeable
		{
		private: // fields
			owned_ptr<gpu*> _device;
			std::size_t _memory_size;
			std::size_t _item_count;
			Private::vulkan_buffer _vulkan_buffer;
			Private::vulkan_memory _vulkan_memory;

		public: // getters
			inline auto device() noexcept -> gpu& { return *_device; }
			inline auto device() const noexcept -> const gpu& { return *_device; }

			inline auto vulkan_memory_size() const noexcept -> std::size_t { return _memory_size; }
			inline auto size() const noexcept { return _item_count; }

			inline auto vulkan_buffer() const noexcept -> Private::vulkan_buffer { return _vulkan_buffer; }
			inline auto vulkan_memory() const noexcept -> Private::vulkan_memory { return _vulkan_memory; }

		public: // constructor
			base_gpu_buffer() = default;
			base_gpu_buffer(base_gpu_buffer&&) = default;
			auto operator=(base_gpu_buffer&&) -> base_gpu_buffer& = default;
			inline ~base_gpu_buffer() noexcept { free(); }

			base_gpu_buffer(gpu& target_device, gpu_buffer_type type, std::size_t item_count, std::size_t item_stride);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_device;
			}
			void free() noexcept final;
		};

		class base_gpu_buffer_data : public basic_freeable
		{
		private: // fields
			owned_ptr<base_gpu_buffer*> _buffer;
			void* _data;

		public: // getters
			auto buffer() noexcept -> base_gpu_buffer& { return *_buffer; }
			auto buffer() const noexcept -> const base_gpu_buffer& { return *_buffer; }

			auto pointer() noexcept -> void* { return _data; }
			auto pointer() const noexcept -> const void* { return _data; }

		public: // constructor
			base_gpu_buffer_data() = default;
			base_gpu_buffer_data(base_gpu_buffer_data&&) = default;
			auto operator=(base_gpu_buffer_data&&)->base_gpu_buffer_data & = default;
			inline ~base_gpu_buffer_data() noexcept { free(); }

			base_gpu_buffer_data(base_gpu_buffer&);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_buffer;
			}
			void free() noexcept final;
		};


		template <typename T>
		class gpu_buffer : public Private::base_gpu_buffer
		{
		public: // fields
			using type = T;

		public: // other methods
			auto data();

		public: // constructor
			gpu_buffer() = default;
			gpu_buffer(gpu_buffer&&) = default;
			auto operator=(gpu_buffer&&) -> gpu_buffer& = default;

			inline gpu_buffer(gpu& target_device, gpu_buffer_type type, std::size_t size)
				: Private::base_gpu_buffer(target_device, type, size, sizeof(T))
			{};

			gpu_buffer(gpu& target_device, gpu_buffer_type type, std::initializer_list<T> data);
		};
#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_BUFFER_TYPE(name, type)									\
		template <typename T>																		\
		class name : public ::CompWolf::Graphics::Private::gpu_buffer<T>							\
		{																							\
		private:																					\
			using super = Private::gpu_buffer<T>;													\
																									\
		public:																						\
			name() = default;																		\
			name(name&&) = default;																	\
			auto operator=(name&&) -> name& = default;												\
																									\
			inline name(::CompWolf::Graphics::gpu& target_device, std::size_t size)					\
				: super(target_device, type, size)													\
			{}																						\
																									\
			inline name(::CompWolf::Graphics::gpu& target_device, std::initializer_list<T> data)	\
				: super(target_device, type, data)													\
			{}																						\
			inline name(::CompWolf::Graphics::gpu& target_device, T data)							\
				: super(target_device, type, { std::move(data) })									\
			{}																						\
		}																							\

	}

	template <typename BufferType>
	class gpu_buffer_data : public std::span<typename BufferType::type, std::dynamic_extent>
	{
	private: // fields
		using super_span = std::span<typename BufferType::type, std::dynamic_extent>;

		Private::base_gpu_buffer_data _data;

	public: // getters
		auto buffer() noexcept -> BufferType& { return *static_cast<BufferType*>(&_data.buffer()); }
		auto buffer() const noexcept -> const BufferType& { return *static_cast<const BufferType*>(&_data.buffer()); }

	public: // constructor
		gpu_buffer_data() = default;
		gpu_buffer_data(gpu_buffer_data&&) = default;
		auto operator=(gpu_buffer_data&&) -> gpu_buffer_data& = default;

		gpu_buffer_data(BufferType& target_buffer)
			: _data(target_buffer)
		{
			super_span::operator=(super_span(static_cast<typename BufferType::type*>(_data.pointer()), target_buffer.size()));
		}
	};


	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_BUFFER_TYPE(gpu_vertex_buffer, Private::gpu_buffer_type::vertex);
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_BUFFER_TYPE(gpu_uniform_buffer, Private::gpu_buffer_type::uniform);

	class gpu_index_buffer : public ::CompWolf::Graphics::Private::gpu_buffer<shader_int>
	{
	private:
		using super = Private::gpu_buffer<shader_int>;

	public:
		gpu_index_buffer() = default;
		gpu_index_buffer(gpu_index_buffer&&) = default;
		auto operator=(gpu_index_buffer&&)->gpu_index_buffer & = default;

		inline gpu_index_buffer(::CompWolf::Graphics::gpu& target_device, std::size_t size)
			: super(target_device, Private::gpu_buffer_type::index, size)
		{}

		inline gpu_index_buffer(::CompWolf::Graphics::gpu& target_device, std::initializer_list<shader_int> data)
			: super(target_device, Private::gpu_buffer_type::index, data)
		{}
	};


	namespace Private
	{
		template <typename T>
		inline auto gpu_buffer<T>::data() { return gpu_buffer_data<gpu_buffer<T>>(*this); }

		template <typename T>
		gpu_buffer<T>::gpu_buffer(gpu& target_device, gpu_buffer_type type, std::initializer_list<T> data_list)
			: gpu_buffer(target_device, type, data_list.size())
		{
			auto dest_buffer = data();

			auto src = data_list.begin();
			for (auto dest = dest_buffer.begin(); dest != dest_buffer.end(); ++dest, ++src)
			{
				new (&*dest) (T) (std::move(*src));
			}
		}
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
