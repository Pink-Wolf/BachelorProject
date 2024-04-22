#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HANDLES_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HANDLES_HEADER

#include "primitive_shader_field.hpp"
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	namespace Private
	{
		template <typename Container, typename PrimitiveTypeList>
		struct primitives_to_info_vector {};

		template <typename Container, PrimitiveShaderField... PrimitiveTypes>
		struct primitives_to_info_vector<Container, type_list<PrimitiveTypes...>>
		{
			static auto info_vector()
				-> const Container&
			{
				static const Container data
				{
					shader_field_info<PrimitiveTypes>::type_info()...
				};
				return data;
			}
		};
	}

	/* Returns the shader_field_info_handles of the primitive shader types making up the given type. Each handle represents a shader_field_info*.
	 * This function specifically returns a reference to a statically constructed object of the given "Container" type; the object is constructed with a begin- and end-iterator.
	 * This reveals a lower level of abstractions, and so should rarely be used.
	 */
	template <ShaderField Type, typename Container>
	static auto shader_field_info_handles()
		-> const Container&
	{
		return Private::primitives_to_info_vector<Container, typename shader_field_info<Type>::primitives>::info_vector();
	}
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HANDLES_HEADER
