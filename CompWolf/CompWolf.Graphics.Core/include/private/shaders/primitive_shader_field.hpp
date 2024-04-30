#ifndef COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER

#include "shader_field_info.hpp"
#include "vulkan_types"
#include <dimensions>
#include <compwolf_utility>
#include "shader_int.hpp"
#include "color.hpp"

namespace CompWolf
{
	template <typename T>
	concept PrimitiveShaderField = ShaderField<T> && requires ()
	{
		{ shader_field_info<T>::type_info() } -> std::same_as<Private::shader_field_info_handle>;
	};

#define COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(type)																											\
	template<> struct shader_field_info<type>																															\
	{																																									\
		/* A list of primitive shader types which makes up the class/struct. */																							\
		using primitives = type_list<type>;																																\
		/* For each of the primitives making up the class/struct, their byte-position relative to the beginning of the class/struct. */									\
		static constexpr const std::array<std::size_t, 1> primitive_offsets = { 0 };																					\
		/* Returns a shader_field_info_handle, representing a shader_field_info*. This reveals a lower level of abstractions, and so this should rarely be used. */		\
		static auto type_info() noexcept -> Private::shader_field_info_handle;																							\
	}																																									\


	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float4);

	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double4);

	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int4);

	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(single_color);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(transparent_color);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(opaque_color);

	/* When used as a type argument in a shader's FieldTypes, denotes that field as an image. */
	struct shader_image {};
	template<> struct shader_field_info<shader_image>
	{
		/* A list of primitive shader types which makes up the class/struct. */
		using primitives = type_list<transparent_color>;
		/* For each of the primitives making up the class/struct, their byte-position relative to the beginning of the class/struct. */
		static constexpr const std::array<std::size_t, 1> primitive_offsets = { 0 };
		/* Returns a shader_field_info_handle, representing a shader_field_info*. This reveals a lower level of abstractions, and so this should rarely be used. */
		static inline auto type_info() noexcept { return shader_field_info<transparent_color>::type_info(); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
