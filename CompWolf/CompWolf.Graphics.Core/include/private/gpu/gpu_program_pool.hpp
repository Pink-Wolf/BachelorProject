#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER

#include "gpu"
#include "vulkan_types"
#include <freeable>
#include <owned>
#include <utility>
#include "type_traits"

namespace CompWolf::Graphics
{
	class gpu_program_pool : public basic_freeable
	{
	public: // fields
		using synchronizations_element_type = std::pair<gpu_fence, gpu_semaphore>;
		using synchronizations_type = std::vector<synchronizations_element_type>;
	private:
		owned_ptr<gpu*> _device;
		size_t _thread_family_index;
		size_t _thread_index;
		Private::vulkan_command_pool _vulkan_pool;

		synchronizations_type _synchronizations;

	public: // getter
		inline auto device() noexcept -> gpu& { return *_device; }
		inline auto device() const noexcept -> const gpu& { return *_device; }

		inline auto thread_family() noexcept -> gpu_thread_family& { return _device->families()[_thread_family_index]; }
		inline auto thread_family() const noexcept -> const gpu_thread_family& { return _device->families()[_thread_family_index]; }

		inline auto thread() noexcept -> gpu_thread& { return thread_family().threads[_thread_index]; }
		inline auto thread() const noexcept -> const gpu_thread& { return thread_family().threads[_thread_index]; }

		inline auto vulkan_pool() const noexcept { return _vulkan_pool; }

		inline auto synchronizations() noexcept -> synchronizations_type& { return _synchronizations; }
		inline auto synchronizations() const noexcept -> const synchronizations_type& { return _synchronizations; }

		inline auto last_vulkan_fence() const noexcept -> Private::vulkan_fence
		{
			if (synchronizations().empty()) return nullptr;
			return synchronizations().back().first.vulkan_fence();
		}

		inline auto last_vulkan_semaphore() const noexcept -> Private::vulkan_semaphore
		{
			if (synchronizations().empty()) return nullptr;
			return synchronizations().back().second.vulkan_semaphore();
		}

	public: // other methods
		template <typename... InputTypes>
			requires std::is_constructible_v<synchronizations_element_type, InputTypes&&...>
		inline auto insert_synchronization(InputTypes&&... inputs) noexcept -> synchronizations_element_type&
		{
			return synchronizations().emplace_back(std::forward<InputTypes>(inputs)...);
		}

	public: // constructors
		gpu_program_pool() = default;
		gpu_program_pool(gpu_program_pool&&) = default;
		auto operator=(gpu_program_pool&&) -> gpu_program_pool& = default;
		~gpu_program_pool() noexcept { free(); }

		gpu_program_pool(gpu& device, size_t thread_family_index, size_t thread_index);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER
