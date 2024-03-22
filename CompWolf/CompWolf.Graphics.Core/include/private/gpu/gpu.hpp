#ifndef COMPWOLF_GRAPHICS_GPU_HEADER
#define COMPWOLF_GRAPHICS_GPU_HEADER

#include "vulkan_types"
#include "gpu_thread.hpp"
#include <utility>
#include <vector>
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	/* Contains a connection to a gpu on the machine that can be used by CompWolf::Graphics. */
	class gpu : public basic_freeable
	{
	private: // fields
		owned_ptr<Private::vulkan_instance> _vulkan_instance;
		/* The gpu. */
		Private::vulkan_physical_device _vulkan_physical_device;
		/* The connection to the gpu. */
		Private::vulkan_device _vulkan_device;
		/* The threads the gpu contains, grouped together in various families. */
		std::vector<gpu_thread_family> _families;
		/* The type of work at least 1 thread on the gpu can perform. */
		gpu_job_type_set _work_types;

	public: // getters
		/* All of the gpu's threads, split into various "families".
		 * On a lower abstraction layer, gets all of the gpu's queues, split into their different queue families.
		 */
		inline std::vector<gpu_thread_family>& families() noexcept
		{
			return _families;
		}
		/* All of the gpu's threads, split into various "families".
		 * On a lower abstraction layer, gets all of the gpu's queues, split into their different queue families.
		 */
		inline const std::vector<gpu_thread_family>& families() const noexcept
		{
			return _families;
		}

		auto index_of_family(const gpu_thread_family&) const -> std::size_t;

		/* The type of work at least 1 thread on the gpu can perform. */
		inline gpu_job_type_set& work_types()
		{
			return _work_types;
		}
		/* The type of work at least 1 thread on the gpu can perform. */
		inline const gpu_job_type_set& work_types() const noexcept
		{
			return _work_types;
		}

		/* The gpu. */
		inline auto vulkan_physical_device() const noexcept
		{
			return _vulkan_physical_device;
		}
		/* The connection to the gpu. */
		inline auto vulkan_device() const noexcept
		{
			return _vulkan_device;
		}
		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
		 * Returns the vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance vulkan_instance() const noexcept
		{
			return _vulkan_instance;
		}

	public: // constructors
		/* Constructs a gpu that is not connected to any actual gpu. */
		gpu() = default;
		gpu(gpu&&) = default;
		gpu& operator=(gpu&&) = default;
		inline ~gpu() noexcept
		{
			free();
		}

		/* Should only be constructed by graphics_environment.
		 * @param device The actual graphics processing unit to make a gpu-object for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		gpu(Private::vulkan_instance vulkan_instance, Private::vulkan_physical_device device);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_instance;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_HEADER
