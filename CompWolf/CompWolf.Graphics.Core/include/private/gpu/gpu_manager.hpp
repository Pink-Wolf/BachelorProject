#ifndef COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
#define COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER

#include "gpu.hpp"
#include "gpu_job.hpp"
#include "../graphics/graphics_environment_settings.hpp"
#include "vulkan_types"
#include <vector>
#include <iterator>
#include <optional>

namespace CompWolf::Graphics
{
	/* Contains a connection to each gpu on the machine that can be used by CompWolf::Graphics. */
	class gpu_manager
	{
	private:
		using _gpus_type = std::vector<gpu>;
		/* The gpus contained by the manager. */
		_gpus_type _gpus;
		/* The amount of thread families the various GPUs have together. */
		size_t _thread_family_count;
	public:
		/* Constructs a manager with no gpus. */
		gpu_manager() = default;
		/* Should only be constructed by graphics_environment.
		 * @param settings How the gpu_manager should behave. The object must stay alive throughout gpu_manager's lifetime.
		 * @param instance The instance of vulkan to create connections to gpus for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program. */
		gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance instance);

	public:
		/* Returns the gpu-connections the manager contains. */
		auto gpu_container() noexcept -> _gpus_type&
		{
			return _gpus;
		}
		/* Returns the gpu-connections the manager contains. */
		auto gpu_container() const noexcept -> const _gpus_type&
		{
			return _gpus;
		}

	public:
		/* Creates a new persistent job. */
		auto new_persistent_job(gpu_job_settings settings) -> gpu_job;
	private:
		auto find_job_family(const gpu_job_settings& settings, bool is_persistent_job) -> std::pair<gpu*, size_t>;
		auto find_job_thread_in_family(const gpu_job_settings& settings, bool is_persistent_job, const gpu_thread_family& family) -> size_t;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
