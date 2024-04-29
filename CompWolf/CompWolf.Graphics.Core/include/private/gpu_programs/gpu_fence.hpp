#ifndef COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
#define COMPWOLF_GRAPHICS_GPU_FENCE_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_connection;

	/* A "fence", which allows the cpu to know when the gpu has finished some work. */
	class gpu_fence : public basic_freeable
	{
	private: // fields
		/* The gpu that the fence is on. */
		const gpu_connection* _device;
		/* The vulkan_fence, representing a VkFence. */
		owned_ptr<Private::vulkan_fence> _vulkan_fence;

	public: // accessors
		/* Returns the gpu that the fence is on. */
		inline auto& gpu() const noexcept { return *_device; }
		/* Returns true if the work is done, otherwise returns false. */
		auto signaled() const noexcept -> bool;

	public: // modifiers
		/* Waits until the work is done, and then returns. */
		void wait() const noexcept;
		/* Makes the fence consider its related work not done. */
		void reset() noexcept;

	public: // vulkan-related
		/* Returns the vulkan_fence, representing a VkFence. */
		inline auto vulkan_fence() const noexcept { return _vulkan_fence; }

	public: // constructors
		/* Constructs a freed gpu_fence, as in one that cannot be used. */
		gpu_fence() = default;
		gpu_fence(gpu_fence&&) = default;
		auto operator=(gpu_fence&&) -> gpu_fence& = default;
		inline ~gpu_fence() noexcept
		{
			free();
		}

		/* Creates a fence for the given gpu.
		 * @param signaled If true, then the fence states that the gpu-work is already finished.
		 * @throws std::runtime_error if there was an error during creation of the fence due to causes outside of the program.
		 */
		gpu_fence(const gpu_connection& target_gpu, bool signaled = false);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_fence;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FENCE_HEADER
