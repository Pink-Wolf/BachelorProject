#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER

#include <concepts>
#include <array>
#include "utility"

namespace CompWolf::Graphics
{
	/* A type trait class for types passed to shaders; That is, to be able to pass your class or struct to a shader, then you must specialize this template with your type. The type would then fulfill ShaderField.
	 * You can use new_shader_field to easily define your specialization.
	 * @typeparam FieldType The type to get info about being passed to shaders. shader_field_info must be properly specialized for the given FieldType, or else are shader_field_info's members not defined.
	 */
	template <typename FieldType>
	struct shader_field_info {};
	/* Whether the given type can be passed to a shader. Specifically whether shader_field_info has been properly specialized for the type. */
	template <typename FieldType>
	concept ShaderField = requires ()
	{
		/* FieldType must have a type_list of primitive shader types, denoting the types that it is made out of. */
		typename shader_field_info<FieldType>::primitives;
		/* FieldType must contain an array that, for each of the primitives making up the FieldType, denotes their byte-position relative to the beginning of the FieldType. */
		{ shader_field_info<FieldType>::primitive_offsets } -> std::same_as<const std::array<std::size_t, shader_field_info<FieldType>::primitives::size>&>;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER
