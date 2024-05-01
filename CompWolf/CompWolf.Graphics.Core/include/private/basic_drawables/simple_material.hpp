#ifndef COMPWOLF_GRAPHICS_SIMPLE_MATERIAL_HEADER
#define COMPWOLF_GRAPHICS_SIMPLE_MATERIAL_HEADER

#include <freeable>
#include "drawables"
#include "simple_vertex_shader.hpp"
#include "single_color_pixel_shader.hpp"
#include <utility>

namespace CompWolf
{
	namespace Private
	{
		struct simple_material_shader_container
		{
			simple_vertex_shader _input_shader;
			single_color_pixel_shader _pixel_shader;
		};
	}

	/* A simple material which contains 2 fields: a float3 color and float2 position. Drawables using this material are moved to the given position, and are drawn with the single color given.
	 * Requires the resource-folder from Graphics.Core to be included with the executable.
	 */
	class simple_material
		: private Private::simple_material_shader_container
		, public draw_material<simple_vertex_shader::shader_type, single_color_pixel_shader::shader_type>
	{
	private: // type alias
		using super_material = draw_material<simple_vertex_shader::shader_type, single_color_pixel_shader::shader_type>;

	public: // constructors
		/* Constructs a freed simple_material, as in one that cannot be used to draw anything. */
		simple_material() = default;
		simple_material(simple_material&&) = default;
		auto operator=(simple_material&&) -> simple_material& = default;
		inline ~simple_material() noexcept { free(); }

		/* Constructs a simple_material with the given settings.
		 * @throws std::runtime_error if the resources could not be found or opened.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		inline simple_material(draw_material_settings settings)
			: Private::simple_material_shader_container(
				{
					simple_vertex_shader(true),
					single_color_pixel_shader(true)
				})
			, super_material(
				Private::simple_material_shader_container::_input_shader.shader(),
				Private::simple_material_shader_container::_pixel_shader.shader(),
				std::move(settings)
			)
		{}

	public: // CompWolf::freeable
		inline void free() noexcept override
		{
			Private::simple_material_shader_container::_input_shader.free();
			Private::simple_material_shader_container::_pixel_shader.free();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_MATERIAL_HEADER
