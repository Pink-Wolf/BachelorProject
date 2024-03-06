#ifndef COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
#define COMPWOLF_GRAPHICS_GPU_FENCE_HEADER

#include "vulkan_types"
#include "gpu.hpp"
#include "gpu_user.hpp"

namespace CompWolf::Graphics
{
	class gpu_fence : public basic_const_gpu_user
	{
	private:
		Private::vulkan_fence _vulkan_fence = nullptr;

	public:
		inline auto empty() const noexcept
		{
			return !_vulkan_fence;
		}

		inline auto vulkan_fence() const noexcept
		{
			return _vulkan_fence;
		}

	public:
		gpu_fence() = default;
		gpu_fence(const gpu_fence&) = delete;
		auto operator=(const gpu_fence&) -> gpu_fence& = delete;
		gpu_fence(gpu_fence&&);
		auto operator=(gpu_fence&&) -> gpu_fence&;

		gpu_fence(const gpu& target_gpu);

		void clear() noexcept;
		inline ~gpu_fence()
		{
			clear();
		}

	public:
		void wait() const;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
