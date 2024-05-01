#include "pch.h"
#include "basic_drawables"

#include <stdexcept>

namespace CompWolf
{
	/******************************** constructors ********************************/

	simple_shape::simple_shape(window& window,
		gpu_index_buffer& indices, gpu_input_buffer<float2>& vertices
		, float3 color, float2 position
	)
		: _indices(&indices), _vertices(&vertices)
		, _color(window.gpu(), color)
		, _position(window.gpu(), position)
		, _material(draw_material_settings())
		, _draw_shape(window, _material, *_indices, *_vertices, _position, _color)
	{
		if (&window.gpu() != &indices.gpu()) throw std::invalid_argument("Tried creating a simple shape with indices on a different gpu than the window.");
		if (&window.gpu() != &vertices.gpu()) throw std::invalid_argument("Tried creating a simple shape with vertices on a different gpu than the window.");
	}

	/******************************** CompWolf::freeable ********************************/

	void simple_shape::free() noexcept
	{
		if (empty()) return;

		_color.free();
		_position.free();
		_material.free();
	}
}
