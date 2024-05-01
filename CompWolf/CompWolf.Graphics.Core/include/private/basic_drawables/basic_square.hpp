#ifndef COMPWOLF_GRAPHICS_BASIC_SQUARE_HEADER
#define COMPWOLF_GRAPHICS_BASIC_SQUARE_HEADER

#include "simple_shape.hpp"

namespace CompWolf
{
	namespace Private
	{
		struct basic_square_buffer_container
		{
			gpu_index_buffer _indices;
			gpu_input_buffer<float2> _vertices;
		};
	}

	/* A drawable square with a color and position, as in a square that is drawn on a window.
	 * The square has a float3 color and float2 position buffer. The center of the square is moved to the given position, and the square is drawn with the single color given.
	 * Requires the resource-folder from Graphics.Core to be included with the executable.
	 */
	class basic_square : private Private::basic_square_buffer_container, public simple_shape
	{
	public: // constructors
		/* Constructs a freed basic_square, as in one that is invisible. */
		basic_square() = default;
		basic_square(basic_square&&) = default;
		auto operator=(basic_square&&) -> basic_square& = default;
		inline ~basic_square() noexcept { free(); }

		/* Constructs a basic_square with the given position and color, drawn on the given window.
		 * @throws std::runtime_error if the resources could not be found or opened.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		basic_square(window&, float3 color, float2 position);

		/* Constructs a basic_square, drawn on the given window.
		 * @throws std::runtime_error if the resources could not be found or opened.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		inline basic_square(window& w)
			: basic_square(w, { 1.f, 1.f, 1.f }, { 0, 0 })
		{}

	public: // CompWolf::freeable
		void free() noexcept override;
	};
}

#endif // ! COMPWOLF_GRAPHICS_BASIC_SQUARE_HEADER
