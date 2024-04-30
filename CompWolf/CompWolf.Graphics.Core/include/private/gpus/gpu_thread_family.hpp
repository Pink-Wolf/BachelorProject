#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER

#include "vulkan_types"
#include "gpu_thread_type.hpp"
#include "gpu_thread.hpp"
#include <vector>
#include <utility>

namespace CompWolf
{
	/* Aggregate type representing a collection of threads on a gpu, where a "thread" is a gpu queue. */
	struct gpu_thread_family
	{
		/* The type of work that the family's threads can perform. */
		gpu_thread_type_set type;

		/* The family's threads, where a "thread" is a gpu queue. */
		std::vector<gpu_thread> threads;

		/* The amount of gpu_program_manager currently on the family's threads. */
		std::size_t program_manager_count;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_FAMILY_HEADER
