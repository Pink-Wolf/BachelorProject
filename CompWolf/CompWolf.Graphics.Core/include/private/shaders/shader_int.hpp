#ifndef COMPWOLF_GRAPHICS_SHADER_INT_HEADER
#define COMPWOLF_GRAPHICS_SHADER_INT_HEADER

#include <dimensions>
#include <cstdint>

namespace CompWolf
{
	/* An int or uint in for example the shader language glsl is 32 bits. This is a 32 bit int to match.
	 * 
	 * There exists dimensions-versions of shader_int defined using COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES.
	 */
	using shader_int = uint32_t;
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(shader_int);
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_INT_HEADER
