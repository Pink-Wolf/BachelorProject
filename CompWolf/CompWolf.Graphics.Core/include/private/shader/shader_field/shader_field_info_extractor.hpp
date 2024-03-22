#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_EXTRACTOR_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_EXTRACTOR_HEADER

#include "primitive_shader_field.hpp"
#include <type_traits>

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

	template <ShaderField Type, typename Container>
	static auto shader_field_info_offsets()
		-> const Container&
	{
		static auto& array_data = shader_field_info<Type>::type_offsets;
		static const Container vector_data(array_data.begin(), array_data.end());
		return vector_data;
	}
	template <ShaderField Type, typename Container>
	static auto shader_field_info_handles()
		-> const Container&
	{
		return Private::primitives_to_info_vector<Container, typename shader_field_info<Type>::types>::info_vector();
	}
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_EXTRACTOR_HEADER
