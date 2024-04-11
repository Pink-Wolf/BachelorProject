#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER

#include "vulkan_types"
#include "gpu_job_settings.hpp"
#include "gpu_thread.hpp"
#include <vector>
#include <utility>

namespace CompWolf::Graphics
{
	/* Aggregate type representing a collection of threads on a gpu, where a "thread" is a gpu queue. */
	struct gpu_thread_family
	{
		/* The type of work that the threads can perform. */
		gpu_job_type_set job_types;

		/* The family's threads, where a "thread" is a gpu queue. */
		std::vector<gpu_thread> threads;

		/* The amount of jobs currently running on any of the family's threads. */
		std::size_t job_count;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER
