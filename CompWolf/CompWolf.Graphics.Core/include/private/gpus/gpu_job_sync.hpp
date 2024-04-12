#ifndef COMPWOLF_GRAPHICS_GPU_JOB_SYNC_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_SYNC_HEADER

#include "gpu_fence.hpp"
#include "gpu_semaphore.hpp"

namespace CompWolf::Graphics
{
	/* Aggregate type containing data used by gpu_job for synchronization between its work. */
	struct gpu_job_sync
	{
		/* Fence waiting for all work to be done. */
		gpu_fence fence;
		/* Semaphore waiting for all work to be done. */
		gpu_semaphore semaphore;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_SYNC_HEADER
