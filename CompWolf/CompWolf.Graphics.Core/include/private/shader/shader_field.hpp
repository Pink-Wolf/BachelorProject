#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_HEADER

#include "vulkan_types"
#include <concepts>
#include <dimensions>
#include <compwolf_type_traits>
#include <array>
#include <vector>
#include <utility>

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

	namespace Private
	{
		template <typename TypeList>
		struct primitives_to_field_info {};
		template <PrimitiveShaderField... PrimitiveTypes>
		struct primitives_to_field_info<type_list<PrimitiveTypes...>>
		{
			static auto constexpr as_vector()
				-> const std::vector<Private::shader_field_info_handle>&
			{
				static const std::vector<Private::shader_field_info_handle> data
				{
					shader_field_info<PrimitiveTypes>::type_info()...
				};
				return data;
			}
		};
	}
	template <typename T>
	auto constexpr shader_field_info_vector()
		-> const std::vector<Private::shader_field_info_handle>&
	{
		return Private::primitives_to_field_info<typename shader_field_info<T>::types>::as_vector();
	}

	template <typename T>
	auto constexpr shader_field_offset_vector()
		-> const std::vector<std::size_t>&
	{
		static auto& array_data = shader_field_info<T>::type_offsets;
		static const std::vector<std::size_t> vector_data(array_data.begin(), array_data.end());
		return vector_data;
	}

	template <TypeValuePair... FieldAndOffsets>
		requires (ShaderField<typename FieldAndOffsets::type> && ...)
	struct shader_field_info_from_fields
	{
	public:
		using type_offsets_type = std::array<std::size_t, (shader_field_info<typename FieldAndOffsets::type>::type_offsets.size() + ...)>;
	private:
		using field_and_offsets = type_list<FieldAndOffsets...>;

		static consteval auto calculate_offsets()
		{
			type_offsets_type offsets;

			std::size_t field_offsets[] = { FieldAndOffsets::value... };
			const std::size_t* type_offsets[] = { shader_field_info<typename FieldAndOffsets::type>::type_offsets.data()...};
			std::size_t type_offsets_size[] = { shader_field_info<typename FieldAndOffsets::type>::type_offsets.size()... };

			std::size_t i = 0;
			for (std::size_t field_index = 0; field_index < field_and_offsets::size; ++field_index)
			{
				auto field_offset = field_offsets[field_index];
				auto field_type_offset = type_offsets[field_index];
				auto field_type_offset_size = type_offsets_size[field_index];

				for (std::size_t type_index = 0; type_index < field_type_offset_size; ++type_index, ++i)
				{
					auto type_offset = field_type_offset[type_index];
					
					offsets[i] = type_offset + field_offset;
				}
			}

			return offsets;
		}
	public:
		using types = combine_type_lists_t<typename shader_field_info<typename FieldAndOffsets::type>::types...>;
		static constexpr const type_offsets_type type_offsets = calculate_offsets();
	};

#define COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(type)					\
	template<> struct shader_field_info<type>									\
	{																			\
		using types = type_list<type>;											\
		static constexpr const std::array<std::size_t, 1> type_offsets = { 0 };		\
		static auto type_info() noexcept -> Private::shader_field_info_handle;	\
	}																			\


	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float2d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float3d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(float4d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double2d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double3d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(double4d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(int);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(int2d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(int3d);
	COMPWOLF_GRAPHICS_DECLARE_PRIMITIVE_SHADER_FIELD(int4d);

}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_HEADER
