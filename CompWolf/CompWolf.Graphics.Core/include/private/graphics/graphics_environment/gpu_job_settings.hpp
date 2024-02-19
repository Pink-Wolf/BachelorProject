#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER

#include <enum_bitset>

namespace CompWolf::Graphics
{
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
		gpu_job_type_set type;
		gpu_job_priority priority;
	};

	/* An identifier for a persistent job on a gpu. */
	using persistent_job_key = size_t;
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_SETTINGS_HEADER
