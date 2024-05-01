#ifndef COMPWOLF_GRAPHICS_SIMPLE_SHAPE_HEADER
#define COMPWOLF_GRAPHICS_SIMPLE_SHAPE_HEADER

#include <freeable>
#include "drawables"
#include "gpu_buffers"
#include "simple_material.hpp"
#include <utility>
#include "windows"

namespace CompWolf
{
	/* A simple drawable which contains 2 buffers: a float3 color and float2 position. This is moved to the given position, and are drawn with the single color given.
	 * Requires the resource-folder from Graphics.Core to be included with the executable.
	 */
	class simple_shape : public basic_freeable
	{
	public: // type definitions
		/* The type of drawable this contains. */
		using drawable_type = drawable<simple_material>;
	private: // fields
		gpu_index_buffer* _indices;
		gpu_input_buffer<float2>* _vertices;

		gpu_field_buffer<float3> _color;
		gpu_field_buffer<float2> _position;

		simple_material _material;
		drawable<simple_material> _draw_shape;

	public: // accessors
		/* Returns the color of this shape. */
		inline auto& color() noexcept { return _color; }
		/* Returns the color of this shape. */
		inline auto& color() const noexcept { return _color; }

		/* Returns the position of this shape. */
		inline auto& position() noexcept { return _position; }
		/* Returns the position of this shape. */
		inline auto& position() const noexcept { return _position; }

	public: // constructors
		/* Constructs a freed simple_shape, as in one that is invisible. */
		simple_shape() = default;
		simple_shape(simple_shape&&) = default;
		auto operator=(simple_shape&&) -> simple_shape& = default;
		inline ~simple_shape() noexcept { free(); }

		/* Constructs a simple_shape with the given index, vertices, position and color, drawn on the given window.
		 * Throws std::runtime_error if the resources could not be found or opened.
		 * Throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		simple_shape(window&,
			gpu_index_buffer& indices, gpu_input_buffer<float2>& vertices
			, float3 color, float2 position
		);

		/* Constructs a simple_shape with the given index and vertices, drawn on the given window.
		 * Throws std::runtime_error if the resources could not be found or opened.
		 * Throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		inline simple_shape(window& window, gpu_index_buffer& indices, gpu_input_buffer<float2>& vertices)
			: simple_shape(window, indices, vertices, { 1.f, 1.f, 1.f }, { 0, 0 })
		{}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _color.empty();
		}
		void free() noexcept override;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_SHAPE_HEADER
