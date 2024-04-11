#ifndef COMPWOLF_GRAPHICS_GPU_JOB_TYPE_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_TYPE_HEADER

#include <utility>
#include <enum_bitset>

namespace CompWolf::Graphics
{
	/* A type of work that a thread on a gpu may be able to do. */
	enum class gpu_job_type : std::size_t
	{
		/* Rendering/drawing images. */
		draw,
		/* Sets what image is visible on a window. */
		present,
		/* The amount of values in the enum, excluding this. */
		size
	};
	/* A set of gpu_job_types, as in some types of work that a thread on a gpu may be able to do. */
	using gpu_job_type_set = enum_bitset<gpu_job_type>;
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_TYPE_HEADER
