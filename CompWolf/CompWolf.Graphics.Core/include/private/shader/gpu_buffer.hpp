#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER

#include "vulkan_types"
#include "gpu"
#include <freeable>
#include <span>
#include <initializer_list>
#include <utility>

namespace CompWolf::Graphics
{
	namespace Private
	{
		class base_gpu_buffer : public basic_freeable
		{
		private: // fields
			owned_ptr<gpu*> _device;
			size_t _memory_size;
			size_t _item_count;
			Private::vulkan_buffer _vulkan_buffer;
			Private::vulkan_memory _vulkan_memory;

		public: // getters
			inline auto device() noexcept -> gpu& { return *_device; }
			inline auto device() const noexcept -> const gpu& { return *_device; }

			inline auto vulkan_memory_size() const noexcept -> size_t { return _memory_size; }
			inline auto size() const noexcept { return _item_count; }

			inline auto vulkan_buffer() const noexcept -> Private::vulkan_buffer { return _vulkan_buffer; }
			inline auto vulkan_memory() const noexcept -> Private::vulkan_memory { return _vulkan_memory; }

		public: // constructor
			base_gpu_buffer() = default;
			base_gpu_buffer(base_gpu_buffer&&) = default;
			auto operator=(base_gpu_buffer&&)->base_gpu_buffer & = default;
			inline ~base_gpu_buffer() noexcept { free(); }

			base_gpu_buffer(gpu& target_device, size_t item_count, size_t item_stride);

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
	}

	template <typename T>
	class gpu_buffer_data;

	template <typename T>
	class gpu_buffer : public Private::base_gpu_buffer
	{
	public: // other methods
		auto data();

	public: // constructor
		gpu_buffer() = default;
		gpu_buffer(gpu_buffer&&) = default;
		auto operator=(gpu_buffer&&)->gpu_buffer & = default;

		inline gpu_buffer(gpu& target_device, size_t size)
			: Private::base_gpu_buffer(target_device, size, sizeof(T))
		{};

		gpu_buffer(gpu& target_device, std::initializer_list<T> data);
	};

	template <typename T>
	class gpu_buffer_data : public std::span<T, std::dynamic_extent>
	{
	private: // fields
		using super_span = std::span<T, std::dynamic_extent>;

		Private::base_gpu_buffer_data _data;

	public: // getters
		auto buffer() noexcept -> gpu_buffer<T>& { return *static_cast<gpu_buffer<T>*>(&_data.buffer()); }
		auto buffer() const noexcept -> const gpu_buffer<T>& { return *static_cast<const gpu_buffer<T>*>(&_data.buffer()); }

	public: // constructor
		gpu_buffer_data() = default;
		gpu_buffer_data(gpu_buffer_data&&) = default;
		auto operator=(gpu_buffer_data&&) -> gpu_buffer_data& = default;

		gpu_buffer_data(gpu_buffer<T>& target_buffer)
			: _data(target_buffer)
		{
			super_span::operator=(std::span<T, std::dynamic_extent>(static_cast<T*>(_data.pointer()), target_buffer.size()));
		}
	};


	template <typename T>
	inline auto gpu_buffer<T>::data() { return gpu_buffer_data<T>(*this); }

	template <typename T>
	gpu_buffer<T>::gpu_buffer(gpu& target_device, std::initializer_list<T> data_list)
		: gpu_buffer(target_device, data_list.size())
	{
		auto dest_buffer = data();

		auto src = data_list.begin();
		for (auto dest = dest_buffer.begin(); dest != dest_buffer.end(); ++dest, ++src)
		{
			new (&*dest) (T) (std::move(*src));
		}
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_HEADER
