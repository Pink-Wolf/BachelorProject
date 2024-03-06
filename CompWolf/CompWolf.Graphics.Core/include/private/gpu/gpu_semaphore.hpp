#ifndef COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER
#define COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER

#include "vulkan_types"
#include "gpu.hpp"
#include "gpu_user.hpp"

namespace CompWolf::Graphics
{
	class gpu_semaphore : public basic_const_gpu_user
	{
	private:
		Private::vulkan_semaphore _vulkan_semaphore = nullptr;

	public:
		inline auto empty() const noexcept
		{
			return !_vulkan_semaphore;
		}

		inline auto vulkan_semaphore() const noexcept
		{
			return _vulkan_semaphore;
		}

	public:
		gpu_semaphore() = default;
		gpu_semaphore(const gpu_semaphore&) = delete;
		auto operator=(const gpu_semaphore&)->gpu_semaphore & = delete;
		gpu_semaphore(gpu_semaphore&&);
		auto operator=(gpu_semaphore&&)->gpu_semaphore&;

		gpu_semaphore(const gpu& target_gpu);

		void clear() noexcept;
		inline ~gpu_semaphore()
		{
			clear();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER
