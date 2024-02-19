#ifndef COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
#define COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER

#include "gpu.hpp"
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
		/* The position of a thread. */
		struct gpu_thread_position
		{
			/* The family containing the thread. */
			gpu_thread_family family;
			/* The index of the thread in the family. */
			size_t index;
		};

	private:
		using _gpus_type = std::vector<gpu>;
		/* The gpus contained by the manager. */
		_gpus_type _gpus;
		/* The amount of thread families the various GPUs have together. */
		size_t _thread_family_count;

		std::vector<std::optional<gpu_thread_position>> persistent_jobs;
		size_t closed_jobs_in_persistent_jobs;
	public:
		/* Constructs a manager with no gpus. */
		gpu_manager() noexcept = default;
		/* Should only be constructed by graphics_environment.
		 * @param settings How the gpu_manager should behave. The object must stay alive throughout gpu_manager's lifetime.
		 * @param instance The instance of vulkan to create connections to gpus for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program. */
		explicit gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance instance);

	public:
		auto new_persistent_job(gpu_job_settings settings) -> persistent_job_key;
		void close_persistent_job(persistent_job_key);
	private:
		auto find_job_family(const gpu_job_settings& settings, bool is_persistent_job) -> gpu_thread_family&;
		auto find_job_thread_in_family(const gpu_job_settings& settings, bool is_persistent_job, const gpu_thread_family& family) -> size_t;
		void occupy_thread_for_job(const gpu_job_settings& settings, bool is_persistent_job, gpu_thread_family& family, size_t index_in_family);
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
