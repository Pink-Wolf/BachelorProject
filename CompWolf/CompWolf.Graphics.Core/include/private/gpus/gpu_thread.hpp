#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_HEADER

#include "vulkan_types"
#include <utility>

namespace CompWolf::Graphics
{
	/* Aggregate type representing a thread on a gpu, where a "thread" is a gpu queue.
	 * A gpu queue is like a thread, in that commands send to the gpu will run on a single queue, and queues can execute commands in parallel.
	 */
	struct gpu_thread
	{
		/* The amount of gpu_program_pool currently on the thread. */
		std::size_t pool_count = 0;
		/* Represents the VkQueue representing the gpu queue. */
		Private::vulkan_queue queue;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_HEADER
