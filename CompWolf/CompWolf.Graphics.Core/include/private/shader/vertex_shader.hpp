#ifndef COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
#define COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER

#include "vulkan_types"
#include "shader.hpp"
#include "shader_field.hpp"

namespace CompWolf::Graphics
{
	template <ShaderField InputType>
	class vertex_shader : public shader
	{
		using shader::shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
