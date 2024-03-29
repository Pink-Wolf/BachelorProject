#ifndef COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER

#include "shader_field_info.hpp"
#include "vulkan_types"
#include <dimensions>

namespace CompWolf::Graphics
{
	template <typename T>
	concept PrimitiveShaderField = ShaderField<T> && requires ()
	{
		{ shader_field_info<T>::type_info() } -> std::same_as<Private::shader_field_info_handle>;
	};

	template<> struct shader_field_info<void>
	{
		using types = type_list<>;
		static constexpr const std::array<std::size_t, 0> type_offsets;
	};

#define COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(type)					\
	template<> struct shader_field_info<type>									\
	{																			\
		using types = type_list<type>;											\
		static constexpr const std::array<std::size_t, 1> type_offsets = { 0 };	\
		static auto type_info() noexcept -> Private::shader_field_info_handle;	\
	}																			\


	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float4);

	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double4);

	using shader_int = uint32_t;
	COMPWOLF_DIMENSION_TYPE(shader_int);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int2);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int3);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(shader_int4);

}

#endif // ! COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
