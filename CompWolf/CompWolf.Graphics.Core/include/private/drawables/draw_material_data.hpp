#ifndef COMPWOLF_GRAPHICS_DRAW_MATERIAL_DATA_HEADER
#define COMPWOLF_GRAPHICS_DRAW_MATERIAL_DATA_HEADER

#include "vulkan_types"
#include <vector>
#include <utility>

namespace CompWolf::Private
{
	class base_shader;

	/* Contains a draw_material's data and logic that is not specific to a gpu or window. */
	struct draw_material_data
	{
		const std::vector<Private::shader_field_info_handle>* input_types;
		const std::vector<std::size_t>* input_offsets;
		std::size_t input_stride;

		enum class field_type
		{
			buffer,
			image,
		};
		std::vector<field_type> field_types;
		std::vector<std::size_t> field_indices;

		std::vector<bool> field_is_input_field;
		std::vector<bool> field_is_pixel_field;

		base_shader* input_shader;
		base_shader* pixel_shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_MATERIAL_DATA_HEADER
