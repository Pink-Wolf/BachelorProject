#ifndef COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
#define COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER

#include "vulkan_types"
#include "shader.hpp"
#include "shader_field.hpp"
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	template <typename VertexType, typename UniformDataTypeList>
	class vertex_shader : public shader<UniformDataTypeList>
	{
		using shader<UniformDataTypeList>::shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
