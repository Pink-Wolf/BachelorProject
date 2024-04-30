#ifndef COMPWOLF_GRAPHICS_GPU_THREAD_TYPE_HEADER
#define COMPWOLF_GRAPHICS_GPU_THREAD_TYPE_HEADER

#include <utility>
#include <enum_bitset>

namespace CompWolf
{
	/* A type of work that a thread on a gpu may be able to do. */
	enum class gpu_thread_type : std::size_t
	{
		/* Rendering/drawing images. */
		draw,
		/* Sets what image is visible on a window. */
		present,
		/* The amount of values in the enum, excluding this. */
		size
	};
	/* A set of gpu_job_types, as in some types of work that a thread on a gpu may be able to do. */
	using gpu_thread_type_set = enum_bitset<gpu_thread_type>;
}

#endif // ! COMPWOLF_GRAPHICS_GPU_THREAD_TYPE_HEADER
