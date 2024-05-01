#ifndef COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHAPE_HEADER
#define COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHAPE_HEADER

#include "shaders"

namespace CompWolf
{
	namespace Private
	{
		constexpr const char simple_vertex_shader_code_path[] = "resources/CompWolf.Graphics.simple_vertex_shader.spv";
	}

	/* A simple input-shader that adds its float2-field to every vertex, but otherwise simply passes along the input-vertices.
	 * Requires the resource-folder from Graphics.Core to be included with the executable.
	 */
	using simple_vertex_shader = static_shader
		< Private::simple_vertex_shader_code_path
		, input_shader<float2, type_value_pair<float2, 0>>
		>;
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHAPE_HEADER
