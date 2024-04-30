#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_CODE_PARAMETERS_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_CODE_PARAMETERS_HEADER

#include "vulkan_types"

namespace CompWolf
{
	/* Aggregate type containing data passed by gpu_program to its code. */
	struct gpu_program_code_parameters
	{
		/* The program's vulkan_command, representing a VkCommandBuffer. */
		Private::vulkan_command command;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_CODE_PARAMETERS_HEADER
