#ifndef COMPWOLF_GRAPHICS_NEW_SHADER_FIELD_HEADER
#define COMPWOLF_GRAPHICS_NEW_SHADER_FIELD_HEADER

#include "shader_field_info.hpp"
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	/* Implements everything a specialization of shader_field_info needs, which means specializations can just inherit from this instead of manually implementing the fields.
	 * @typeparam FieldAndOffsets Must be some type_value_pairs, specifying the target type's various data members. The pairs should specifically specify their type and offset (offset can be calculated with offsetof).
	 * Only fields that need to be visible by the shader needs to be specified.
	 * All specified types must be ShaderFields themself.
	 * If any of this is not fulfilled, then a compile-time error is issued.
	 */
	template <TypeValuePair... FieldAndOffsets>
		requires (ShaderField<typename FieldAndOffsets::type> && ...)
	struct new_shader_field
	{
	private:
		using type_offsets_type = std::array<std::size_t, (shader_field_info<typename FieldAndOffsets::type>::primitive_offsets.size() + ...)>;
	private:
		using field_and_offsets = type_list<FieldAndOffsets...>;

		static consteval auto calculate_offsets()
		{
			type_offsets_type offsets;

			std::size_t field_offsets[] = { FieldAndOffsets::value... };
			const std::size_t* primitive_offsets[] = { shader_field_info<typename FieldAndOffsets::type>::primitive_offsets.data()... };
			std::size_t primitive_offsets_size[] = { shader_field_info<typename FieldAndOffsets::type>::primitive_offsets.size()... };

			std::size_t i = 0;
			for (std::size_t field_index = 0; field_index < field_and_offsets::size; ++field_index)
			{
				auto field_offset = field_offsets[field_index];
				auto field_primitive_offset = primitive_offsets[field_index];
				auto field_primitive_offset_size = primitive_offsets_size[field_index];

				for (std::size_t primitive_index = 0; primitive_index < field_primitive_offset_size; ++primitive_index, ++i)
				{
					auto primitive_offset = field_primitive_offset[primitive_index];

					offsets[i] = primitive_offset + field_offset;
				}
			}

			return offsets;
		}
	public:
		/* A list of primitive shader types which makes up the class/struct. */
		using primitives = combine_type_lists_t<typename shader_field_info<typename FieldAndOffsets::type>::primitives...>;
		/* For each of the primitives making up the class/struct, their byte-position relative to the beginning of the class/struct. */
		static constexpr const type_offsets_type primitive_offsets = calculate_offsets();
	};

}

#endif // ! COMPWOLF_GRAPHICS_NEW_SHADER_FIELD_HEADER
