#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_SYNC_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_SYNC_HEADER

#include "gpu_fence.hpp"
#include "gpu_semaphore.hpp"

namespace CompWolf
{
	/* Aggregate type containing data used by gpu_program_manager for synchronization between its work. */
	struct gpu_program_sync
	{
		/* Fence waiting for all work to be done. */
		gpu_fence fence;
		/* Semaphore waiting for all work to be done. */
		gpu_semaphore semaphore;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_SYNC_HEADER
