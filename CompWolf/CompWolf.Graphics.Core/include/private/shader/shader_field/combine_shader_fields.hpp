#ifndef COMPWOLF_GRAPHICS_COMBINE_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_COMBINE_SHADER_FIELD_HEADER

#include "shader_field_info.hpp"
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	template <TypeValuePair... FieldAndOffsets>
		requires (ShaderField<typename FieldAndOffsets::type> && ...)
	struct combine_shader_fields
	{
	public:
		using type_offsets_type = std::array<std::size_t, (shader_field_info<typename FieldAndOffsets::type>::type_offsets.size() + ...)>;
	private:
		using field_and_offsets = type_list<FieldAndOffsets...>;

		static consteval auto calculate_offsets()
		{
			type_offsets_type offsets;

			std::size_t field_offsets[] = { FieldAndOffsets::value... };
			const std::size_t* type_offsets[] = { shader_field_info<typename FieldAndOffsets::type>::type_offsets.data()... };
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

}

#endif // ! COMPWOLF_GRAPHICS_COMBINE_SHADER_FIELD_HEADER
