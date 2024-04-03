#ifndef COMPWOLF_GRAPHICS_GPU_MEMORY_HEADER
#define COMPWOLF_GRAPHICS_GPU_MEMORY_HEADER

#include "vulkan_types"
#include "gpu"
#include "shader_field.hpp"
#include <freeable>
#include <span>
#include <initializer_list>
#include <utility>
#include <exception>
#include <variant>
#include <tuple>
#include <functional>

namespace CompWolf::Graphics
{
	struct gpu_memory_allocator
	{
	public: // fields
		gpu* target_device;

		struct data_handle_t {}; using data_handle = Private::vulkan_handle64<data_handle_t>;
		struct private_info_handle_t {}; using private_info_handle = private_info_handle_t*;

	public: // other methods
		virtual auto alloc_data() const->data_handle = 0;
		virtual void bind_data(data_handle, Private::vulkan_memory) const = 0;
		virtual void free_data(data_handle) const noexcept = 0;
		virtual void private_info(data_handle data, private_info_handle out) const = 0;
	public: // constructors
		gpu_memory_allocator() = default;
		gpu_memory_allocator(const gpu_memory_allocator&) = default;
		auto operator=(const gpu_memory_allocator&) -> gpu_memory_allocator& = default;
		gpu_memory_allocator(gpu_memory_allocator&&) = default;
		auto operator=(gpu_memory_allocator&&) -> gpu_memory_allocator& = default;

		gpu_memory_allocator(gpu& device) : target_device(&device) {}
	};
	
	class gpu_memory : public basic_freeable
	{
	public: // fields
		struct bind_handle_t {}; using bind_handle = bind_handle_t*;
		using bind_to_shader_function = std::function<void(bind_handle)>;
	private:
		owned_ptr<gpu*> _device;
		gpu_memory_allocator* _allocator;
		bind_to_shader_function _bind_to_shader;

		std::size_t _item_count;
		std::size_t _memory_size;

		gpu_memory_allocator::data_handle _vulkan_data;
		Private::vulkan_memory _vulkan_memory;

	public: // getters
		inline auto& device() noexcept { return *_device; }
		inline auto& device() const noexcept { return *_device; }

		inline auto allocator() noexcept { return _allocator; }
		inline auto allocator() const noexcept { return _allocator; }

		inline auto size() const noexcept { return _item_count; }
		inline auto vulkan_memory_size() const noexcept -> std::size_t { return _memory_size; }

		inline auto vulkan_data() const { return _vulkan_data; }
		inline auto vulkan_memory() const noexcept -> Private::vulkan_memory { return _vulkan_memory; }

	public: // other methods
		void bind_to_shader(bind_handle handle) const { _bind_to_shader(handle); }

	public: // constructor
		gpu_memory() = default;
		gpu_memory(gpu_memory&&) = default;
		auto operator=(gpu_memory&&) -> gpu_memory& = default;
		inline ~gpu_memory() noexcept { free(); }

		gpu_memory(gpu_memory_allocator* allocator, bind_to_shader_function bind_to_shader);
		template <typename DataType>
		gpu_memory(gpu_memory_allocator* allocator, bind_to_shader_function bind_to_shader, std::initializer_list<DataType> data);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};

	namespace Private
	{

		class base_gpu_memory_access : public basic_freeable
		{
		private: // fields
			owned_ptr<gpu_memory*> _memory;
			void* _data;

		public: // getters
			auto& memory() noexcept { return *_memory; }
			auto& memory() const noexcept { return *_memory; }

			auto pointer() noexcept { return _data; }
			auto pointer() const noexcept { return _data; }

		public: // constructor
			base_gpu_memory_access() = default;
			base_gpu_memory_access(base_gpu_memory_access&&) = default;
			auto operator=(base_gpu_memory_access&&) -> base_gpu_memory_access& = default;
			inline ~base_gpu_memory_access() noexcept { free(); }

			explicit base_gpu_memory_access(gpu_memory&);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_memory;
			}
			void free() noexcept final;
		};
	}

	template <typename ValueType>
	class gpu_memory_access : public std::span<ValueType, std::dynamic_extent>
	{
	private: // fields
		using super_span = std::span<ValueType, std::dynamic_extent>;

		Private::base_gpu_memory_access _data;

	public: // getters
		auto& memory() noexcept { return *static_cast<ValueType*>(&_data.memory()); }
		auto& memory() const noexcept { return *static_cast<const ValueType*>(&_data.memory()); }

	public: // constructor
		gpu_memory_access() = default;
		gpu_memory_access(gpu_memory_access&&) = default;
		auto operator=(gpu_memory_access&&)->gpu_memory_access & = default;

		explicit gpu_memory_access(gpu_memory& target_memory)
			: _data(target_memory)
		{
			super_span::operator=(super_span(static_cast<ValueType*>(_data.pointer()), target_memory.size()));
		}
	};
	template <typename ValueType>
	class gpu_single_memory_access
	{
	public: // fields
		using value_type = ValueType;
	private:
		Private::base_gpu_memory_access _data;

	public: // getters
		auto& memory() noexcept { return *static_cast<value_type*>(&_data.memory()); }
		auto& memory() const noexcept { return *static_cast<const value_type*>(&_data.memory()); }

		auto operator->() noexcept { return static_cast<value_type*>(_data.pointer()); }
		auto operator->() const noexcept { return static_cast<const value_type*>(_data.pointer()); }

		auto& operator*() noexcept { return *this->operator->(); }
		auto& operator*() const noexcept { return *this->operator->(); }

	public: // constructor
		gpu_single_memory_access() = default;
		gpu_single_memory_access(gpu_single_memory_access&&) = default;
		auto operator=(gpu_single_memory_access&&)->gpu_single_memory_access & = default;

		explicit gpu_single_memory_access(gpu_memory& target_memory)
			: _data(target_memory)
		{
			if (target_memory.size() != 1)
				throw std::invalid_argument("Tried creating a gpu_single_memory_access for a memory that did not have exactly 1 element.");
		}
	};

	template <typename ValueType>
	gpu_memory::gpu_memory(gpu_memory_allocator* allocator, bind_to_shader_function bind_to_shader, std::initializer_list<ValueType> data)
		: gpu_memory(allocator, bind_to_shader)
	{
		auto destination_data = gpu_memory_access<ValueType>(*this);

		auto source = data.begin();
		for (auto destination = destination_data.begin()
			; destination != destination_data.end() && source != data.end()
			; ++source, ++destination
			)
		{
			new (&*destination) (ValueType) (std::move(*source));
		}
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MEMORY_HEADER
