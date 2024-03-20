#ifndef COMPWOLF_GRAPHICS_SHADER_FIELD_TYPE_HEADER
#define COMPWOLF_GRAPHICS_SHADER_FIELD_TYPE_HEADER

#include "compwolf_vulkan.hpp"

namespace CompWolf::Graphics::Private
{
	struct shader_field_info
	{
		VkFormat format;
	};

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_POINTER_CONVERTERS(private, shader_field_info*, shader_field_info_handle_t*)
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_FIELD_TYPE_HEADER
