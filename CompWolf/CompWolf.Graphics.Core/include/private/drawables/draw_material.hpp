#ifndef COMPWOLF_GRAPHICS_DRAW_MATERIAL_HEADER
#define COMPWOLF_GRAPHICS_DRAW_MATERIAL_HEADER

#include "draw_material_settings.hpp"
#include "vulkan_types"
#include <freeable>
#include <owned>
#include "draw_material_data.hpp"
#include "gpu_specific_material.hpp"
#include "window_specific_material.hpp"
#include <compwolf_type_traits>
#include <map>
#include <vector>
#include "windows"
#include "shaders"
#include <utility>
#include <concepts>

namespace CompWolf
{
	class gpu_connection;

	namespace Private
	{
		template <typename T>
		struct material_field_types {};
		template <typename... Ts>
		struct material_field_types<type_list<Ts...>>
		{
			static inline std::vector<draw_material_data::field_type> value = {
				std::same_as<typename Ts::type, shader_image>
				? draw_material_data::field_type::image
				: draw_material_data::field_type::buffer
				...
			};
		};

		template <typename T>
		struct material_field_index_transform {};
		template <typename... Ts>
		struct material_field_index_transform<type_list<Ts...>>
		{
			static inline std::vector<std::size_t> value = { Ts::value... };
		};

		template <typename Shader, typename TList>
		struct is_field_part_of_shader {};
		template <typename Shader, typename... Ts>
		struct is_field_part_of_shader<Shader, type_list<Ts...>>
		{
			static inline std::vector<bool> value = { Shader::field_indices.contains(Ts::value)...};
		};
	}

	/* Contains the code/intructions for how to draw things.
	 * @typeparam InputShaderType The type of shader run for each input/vertex of the model.
	 * As in the vertex shader that the material's graphics-pipeline uses.
	 * @typeparam PixelShaderType The type of shader run for each pixel drawn.
	 * As in the fragment shader that the material's graphics-pipeline uses.
	 */
	template <typename InputShaderType, typename PixelShaderType>
	class draw_material : public basic_freeable
	{
	public: // type definitions
		/* The type of shader run for each input/vertex of the model.
		 * As in the vertex shader that the material's graphics-pipeline uses.
		 */
		using input_shader_type = InputShaderType;
		/* The type of shader run for each pixel drawn.
		 * As in the fragment shader that the material's graphics-pipeline uses.
		 */
		using pixel_shader_type = PixelShaderType;

		/* A collection of type_value_pairs, denoting the material's shaders' fields' type and binding position.
		 * If multiple shaders share fields, then they are NOT repeated here.
		 */
		using shader_field_types = merge_type_value_pairs_by_value_t<typename input_shader_type::field_types, typename pixel_shader_type::field_types>;

	private: // fields
		owned_ptr<input_shader_type*> _input_shader;
		pixel_shader_type* _pixel_shader;

		Private::draw_material_data _data;
		std::map<const gpu_connection*, Private::gpu_specific_material> _gpu_data;
		std::map<const window*, window_specific_material> _window_data;

	public: // accessors
		/* Returns the input_shader used by the material's graphics-pipeline as its vertex shader. */
		inline auto& input_shader() noexcept { return *_input_shader; }
		/* Returns the input_shader used by the material's graphics-pipeline as its vertex shader. */
		inline auto& input_shader() const noexcept { return *_input_shader; }

		/* Returns the pixel_shader used by the material's graphics-pipeline as its fragment shader. */
		inline auto& pixel_shader() noexcept { return *_pixel_shader; }
		/* Returns the pixel_shader used by the material's graphics-pipeline as its fragment shader. */
		inline auto& pixel_shader() const noexcept { return *_pixel_shader; }

	public: // vulkan-related
		/* Returns data about drawing with the material on the specific window. */
		auto vulkan_window_data(window& target_window) -> window_specific_material&
		{
			// Get data if it does exist
			{
				auto iterator = _window_data.find(&target_window);
				if (iterator != _window_data.end()) return iterator->second;
			}
			// Create data if it does not exist
			{
				auto& gpu_data = _gpu_data.try_emplace(&target_window.gpu(), _data, target_window.gpu()).first->second;
				auto& window_data = _window_data.try_emplace(&target_window, _data, gpu_data, target_window).first->second;
				return window_data;
			}
		}
		/* Returns data about drawing with the material on the specific window. */
		inline auto vulkan_window_data(const window& w) const -> const window_specific_material&
		{
			return const_cast<draw_material*>(this)->vulkan_window_data(w);
		}

	public: // constructors
		/* Constructs a freed draw_material, as in one that cannot be used to draw anything. */
		draw_material() = default;
		draw_material(draw_material&&) = default;
		auto operator=(draw_material&&) -> draw_material& = default;
		inline ~draw_material() noexcept { free(); }

		/* Constructs a material with the given shaders and settings. */
		draw_material(InputShaderType& input_shader, PixelShaderType& pixel_shader, draw_material_settings settings)
			: _input_shader(&input_shader), _pixel_shader(&pixel_shader), _data(Private::draw_material_data
				{
					.input_types = &shader_field_info_handles<typename input_shader_type::input_type, std::vector<Private::shader_field_info_handle>>(),
					.input_offsets = &shader_field_info_offsets<typename input_shader_type::input_type, std::vector<std::size_t>>(),
					.input_stride = sizeof(typename input_shader_type::input_type),
					.field_types = Private::material_field_types<shader_field_types>::value,
					.field_indices = Private::material_field_index_transform<shader_field_types>::value,
					.field_is_input_field = Private::is_field_part_of_shader<input_shader_type, shader_field_types>::value,
					.field_is_pixel_field = Private::is_field_part_of_shader<pixel_shader_type, shader_field_types>::value,
					.input_shader = &input_shader,
					.pixel_shader = &pixel_shader,
				}
		)
		{}

		/* Constructs a material with the given shaders. */
		draw_material(InputShaderType& input_shader, PixelShaderType& pixel_shader)
			: draw_material(input_shader, pixel_shader, draw_material_settings
				{
					
				}
				) {}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_input_shader;
		}
		inline void free() noexcept final { _input_shader = nullptr; }
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_MATERIAL_HEADER
