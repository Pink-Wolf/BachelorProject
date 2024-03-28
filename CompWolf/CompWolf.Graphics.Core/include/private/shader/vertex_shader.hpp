#ifndef COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
#define COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER

#include "vulkan_types"
#include "shader.hpp"
#include "shader_field.hpp"
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	template <typename VertexTypeList, typename UniformDataTypeList>
	class vertex_shader
	{
		static_assert(std::same_as<vertex_shader, vertex_shader>,
			"vertex_shader was not given type_lists of vertex- and uniform-data-types"
			);
	};
	template <typename... VertexTypes, typename... UniformDataTypes>
	class vertex_shader<type_list<VertexTypes...>, type_list<UniformDataTypes...>> : public shader
	{
		using shader::shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VERTEX_SHADER_HEADER
