#ifndef COMPWOLF_GRAPHICS_UNIFORM_SHADER_HEADER
#define COMPWOLF_GRAPHICS_UNIFORM_SHADER_HEADER

#include "shader.hpp"

namespace CompWolf::Graphics
{
	template <typename UniformDataTypeList>
	class uniform_shader : public shader<UniformDataTypeList>
	{
		using shader<UniformDataTypeList>::shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_UNIFORM_SHADER_HEADER
