#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER

#include <compwolf_type_traits>
#include <concepts>
#include <array>
#include "utility"

namespace CompWolf::Graphics
{
	template <typename T>
	struct shader_field_info
	{

	};
	template <typename T>
	concept ShaderField = requires ()
	{
		typename shader_field_info<T>::types;
		{ shader_field_info<T>::type_offsets } -> std::same_as<const std::array<std::size_t, shader_field_info<T>::types::size>&>;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_INFO_HEADER
