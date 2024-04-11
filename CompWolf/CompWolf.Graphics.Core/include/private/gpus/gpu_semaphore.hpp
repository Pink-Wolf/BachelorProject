#ifndef COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER
#define COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER

#include "vulkan_types"
#include "gpu_connection.hpp"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_semaphore : public basic_freeable
	{
	private: // fields
		const gpu_connection* _device;
		owned_ptr<Private::vulkan_semaphore> _vulkan_semaphore;

	public: // getters
		inline auto device() const noexcept -> const gpu_connection&
		{
			return *_device;
		}
		inline auto vulkan_semaphore() const noexcept
		{
			return _vulkan_semaphore;
		}

	public: // constructors
		gpu_semaphore() = default;
		gpu_semaphore(gpu_semaphore&&) = default;
		auto operator=(gpu_semaphore&&) -> gpu_semaphore& = default;
		inline ~gpu_semaphore() noexcept
		{
			free();
		}

		gpu_semaphore(const gpu_connection& target_gpu);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_semaphore;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER
