#ifndef COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER_HEADER
#define COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER_HEADER

#include <freeable>
#include "shaders"

namespace CompWolf
{
	namespace Private
	{
		constexpr const char single_color_pixel_shader_code_path[] = "resources/CompWolf.Graphics.single_color_pixel_shader.spv";
	}

	/* A simple pixel-shader that colors the object a single color specified by its float3-field.
	 * Requires the resource-folder from Graphics.Core to be included with the executable.
	 */
	using single_color_pixel_shader = static_shader
		< Private::single_color_pixel_shader_code_path
		, shader<type_value_pair<float3, 4>>
		>;
}

#endif // ! COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER_HEADER
