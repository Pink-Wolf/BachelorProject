#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_OFFSETS_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_OFFSETS_HEADER

#include "primitive_shader_field.hpp"

namespace CompWolf::Graphics
{
	/* Returns the offsets of the primitive shader types making up the given type.
	 * This function specifically returns a reference to a statically constructed object of the given "Container" type; the object is constructed with a begin- and end-iterator.
	 */
	template <ShaderField Type, typename Container>
	static auto shader_field_info_offsets()
		-> const Container&
	{
		static auto& array_data = shader_field_info<Type>::primitive_offsets;
		static const Container vector_data(array_data.begin(), array_data.end());
		return vector_data;
	}
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_OFFSETS_HEADER
