#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_HEADER

#include "vulkan_types"
#include "gpu_job_settings.hpp"
#include <vector>
#include <utility>

namespace CompWolf::Graphics
{
	enum class gpu_thread_occupation
	{
		/* The thread is currently not being used. */
		free,
		/* The thread is currently being used by a job, but does not a persistent job. */
		occupied,
		/* The thread is for a persistent job. */
		persistent_occupied,
	};

	/* A thread on a gpu.
	 * On a lower abstraction layer, a queue.
	 */
	struct gpu_thread
	{
		/* The amount of jobs running on the thread. */
		std::size_t job_count = 0;
		/* The amount of persistent jobs running on the thread. */
		std::size_t persistent_job_count = 0;

		Private::vulkan_queue queue;
	};

	/* A collection of thread which can do the same type of work.
	 * On a lower abstraction layer, a queue family.
	 */
	struct gpu_thread_family
	{
		/* The type of work the threads can do. */
		gpu_job_type_set job_types;

		/* How the family's threads are currently being used. */
		std::vector<gpu_thread> threads;

		/* The amount of threads in the family occupied by a persistent job. */
		std::size_t persistent_job_count;
		/* The amount of threads in the family occupied by a job. */
		std::size_t job_count;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_HEADER
