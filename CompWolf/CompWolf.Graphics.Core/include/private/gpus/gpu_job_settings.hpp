#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER

#include "gpu_job_type.hpp"
#include <functional>
#include <optional>

namespace CompWolf::Graphics
{
	class gpu_connection;
	struct gpu_thread_family;

	/* Aggregate type used by gpu_manager.new_job to specify the job to create. */
	struct gpu_job_settings
	{
		/* The type of work the job should be able to perform. */
		gpu_job_type_set type;
		/* Used on each thread's gpu to determine what thread to use.
		 * This and family_scorer's scores are summed to determine the threads' scores; the thread with the highest score is generally picked.
		 * gpu_manager.new_job may still pick a lower-scoring thread, for example when the highest scoring thread is already busy with work.
		 * If this returns std::nullopt, no thread on the given gpu will be picked; this should only be done if the gpu actually cannot perform the work.
		 * If empty, gpu_manager.new_job will score the gpus 0.
		 */
		std::function<std::optional<float>(const gpu_connection&)> gpu_scorer;
		/* Used on each thread's family to determine what thread to use.
		 * This and gpu_scorer's scores are summed to determine the threads' scores; the thread with the highest score is generally picked.
		 * gpu_manager.new_job may still pick a lower-scoring thread, for example when the highest scoring thread is already busy with work.
		 * If this returns std::nullopt, no thread in the given family will be picked; this should only be done if the family actually cannot perform the work.
		 * If empty, gpu_manager.new_job will score the families 0.
		 */
		std::function<std::optional<float>(const gpu_thread_family&)> family_scorer;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
