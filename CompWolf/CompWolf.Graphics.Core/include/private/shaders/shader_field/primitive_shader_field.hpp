#ifndef COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER

#include "shader_field_info.hpp"
#include "vulkan_types"
#include <dimensions>
#include <compwolf_utility>

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

	struct transparent_color;
	struct opaque_color : dimensions<uint8_t, 3>
	{
		auto& r() { return x(); } auto& r() const { return x(); }
		auto& g() { return y(); } auto& g() const { return y(); }
		auto& b() { return z(); } auto& b() const { return z(); }

		operator transparent_color() const;
	};
	struct transparent_color : dimensions<uint8_t, 4>
	{
		auto& r() { return x(); } auto& r() const { return x(); }
		auto& g() { return y(); } auto& g() const { return y(); }
		auto& b() { return z(); } auto& b() const { return z(); }
		auto& a() { return w(); } auto& a() const { return w(); }

		explicit operator opaque_color() const;
	};
	inline opaque_color::operator transparent_color() const { return transparent_color{ r(), g(), b(), 1_uint8 }; }
	inline transparent_color::operator opaque_color() const { return opaque_color{ r(), g(), b() }; }
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(transparent_color);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(opaque_color);

	struct shader_image {};
	template<> struct shader_field_info<shader_image>
	{
		using types = type_list<transparent_color>;
		static constexpr const std::array<std::size_t, 1> type_offsets = { 0 };
		static inline auto type_info() noexcept { return shader_field_info<transparent_color>::type_info(); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_PRIMITIVE_SHADER_FIELD_HEADER
