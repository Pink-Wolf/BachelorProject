#ifndef COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
#define COMPWOLF_GRAPHICS_GPU_FENCE_HEADER

#include "vulkan_types"
#include "gpu.hpp"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_fence : public basic_freeable
	{
	private: // fields
		const gpu* _device;
		owned_ptr<Private::vulkan_fence> _vulkan_fence;

	public: // getters
		inline auto device() const noexcept -> const gpu&
		{
			return *_device;
		}
		inline auto vulkan_fence() const noexcept
		{
			return _vulkan_fence;
		}

		bool signaled() const;

	public: // other methods
		void wait() const;
		void reset();

	public: // constructors
		gpu_fence() = default;
		gpu_fence(gpu_fence&&) = default;
		auto operator=(gpu_fence&&) -> gpu_fence& = default;
		inline ~gpu_fence() noexcept
		{
			free();
		}

		gpu_fence(const gpu& target_gpu, bool signaled = false);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_fence;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
