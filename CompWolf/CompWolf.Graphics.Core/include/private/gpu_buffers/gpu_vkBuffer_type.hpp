#ifndef COMPWOLF_GRAPHICS_GPU_VKBUFFER_TYPE_HEADER
#define COMPWOLF_GRAPHICS_GPU_VKBUFFER_TYPE_HEADER

namespace CompWolf::Graphics
{
	/* The type of buffer that gpu_vkBuffer_allocator is. */
	enum class gpu_vkBuffer_type
	{
		/* An index buffer, as in one whose data specifies which, of the given inputs, an input_shader should use and in what order. */
		index,
		/* A vertex buffer, as in one that contains the inputs to an input_shader. */
		input,
		/* An uniform buffer, as in one that contains a field of a shader. */
		uniform,
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_VKBUFFER_TYPE_HEADER
