#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER

#include <enum_bitset>
#include <functional>
#include <optional>

namespace CompWolf::Graphics
{
	class gpu;
	struct gpu_thread_family;

	/* The different type of work a job for a gpu can be. */
	enum class gpu_job_type : size_t
	{
		/* Drawing on a window or image. */
		draw,
		/* Make a window actually show what has been drawn on it. */
		present,
		size
	};
	/* A combination of gpu_job_types. */
	using gpu_job_type_set = enum_bitset<gpu_job_type>;

	/* The different priorities a job for a gpu can have. */
	enum struct gpu_job_priority
	{
		/* The job should only be performed when it will not impact other jobs. */
		low,
		/* The priority of most jobs. */
		medium,
		/* The job should be performed as quickly as possible, making other jobs wait even if they were started earlier. */
		high,
	};

	/* The specifications for a job for a gpu. */
	struct gpu_job_settings
	{
		/* The type of work the job should be able to do. */
		gpu_job_type_set type;
		/* The priority of the job. */
		gpu_job_priority priority;
		/* How good a given gpu is for this job.
		 * A higher scoring gpu will be picked over a lower scoring one; one without a score will never be picked.
		 * This score is combined with family_scorer.
		 * May be empty to give all gpus the same score.
		 * @see family_scorer
		 */
		std::function<std::optional<float>(const gpu&)> gpu_scorer;
		/* How good a given gpu-thread-family is for this job.
		 * A higher scoring family will be picked over a lower scoring one; one without a score will never be picked.
		 * This score is combined with gpu_scorer. Specfifically, a family's score is the sum of its own score and its gpu's score.
		 * May be empty to give all gpus the same score.
		 * @see gpu_scorer
		 */
		std::function<std::optional<float>(const gpu_thread_family&)> family_scorer;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
