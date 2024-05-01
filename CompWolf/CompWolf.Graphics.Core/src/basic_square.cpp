#include "pch.h"
#include "basic_drawables"

namespace CompWolf
{
	/******************************** constructors ********************************/

	basic_square::basic_square(window& window, float3 color, float2 position)
		: basic_square_buffer_container{
				gpu_index_buffer(window.gpu(), { 0, 1, 2, 3, 0, 2 }),
				gpu_input_buffer<float2>(window.gpu(), {
					{ -.1f, -.1f },
					{ +.1f, -.1f },
					{ +.1f, +.1f },
					{ -.1f, +.1f }
					})
			}
		, simple_shape(window
			, basic_square_buffer_container::_indices, basic_square_buffer_container::_vertices
			, color, position)
	{}

	/******************************** CompWolf::freeable ********************************/

	void basic_square::free() noexcept
	{
		if (empty()) return;

		basic_square_buffer_container::_indices.free();
		basic_square_buffer_container::_vertices.free();
		simple_shape::free();
	}
}
