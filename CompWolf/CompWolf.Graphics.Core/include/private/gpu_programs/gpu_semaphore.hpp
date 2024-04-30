#ifndef COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER
#define COMPWOLF_GRAPHICS_GPU_SEMAPHORE_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>

namespace CompWolf
{
	class gpu_connection;

	/* A "semaphore", which allows synchronization between work on the gpu. */
	class gpu_semaphore : public basic_freeable
	{
	private: // fields
		/* The gpu that the semaphore is on. */
		const gpu_connection* _device;
		/* The vulkan_semaphore, representing a VkSemaphore. */
		owned_ptr<Private::vulkan_semaphore> _vulkan_semaphore;

	public: // accessors
		/* Returns the gpu that the semaphore is on. */
		inline auto& gpu() const noexcept { return *_device; }

	public: // vulkan-related
		/* Returns the vulkan_semaphore, representing a VkSemaphore. */
		inline auto vulkan_semaphore() const noexcept { return _vulkan_semaphore; }

	public: // constructors
		/* Constructs a freed gpu_semaphore, as in one that cannot be used. */
		gpu_semaphore() = default;
		gpu_semaphore(gpu_semaphore&&) = default;
		auto operator=(gpu_semaphore&&) -> gpu_semaphore& = default;
		inline ~gpu_semaphore() noexcept
		{
			free();
		}

		/* Creates a semaphore for the given gpu.
		 * @throws std::runtime_error if there was an error during creation of the semaphore due to causes outside of the program.
		 */
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
