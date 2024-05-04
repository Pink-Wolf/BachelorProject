#ifndef COMPWOLF_GRAPHICS_DRAWABLE_HEADER
#define COMPWOLF_GRAPHICS_DRAWABLE_HEADER

#include "vulkan_types"
#include "windows"
#include "gpu_buffers"
#include "gpu_programs"
#include <freeable>
#include <owned>
#include "draw_material.hpp"
#include "window_specific_material.hpp"
#include <tuple>
#include <concepts>

namespace CompWolf
{
	class base_drawable : public basic_freeable, public window_user
	{
	private: // fields
		window_specific_material* _material;
		gpu_index_buffer* _indices;
		Private::base_gpu_vkBuffer_buffer* _inputs;
		std::vector<base_gpu_buffer*> _fields;

		event<draw_code_parameters&>::key_type _draw_code_key;

	public: // accessors
		/* Returns the material used to draw the drawable object. */
		inline auto& material() noexcept { return *_material; }
		/* Returns the material used to draw the drawable object. */
		inline auto& material() const noexcept { return *_material; }

	private: // event handler
		void draw_program_code(draw_code_parameters& args);

	public: // constructors
		base_drawable() = default;
		base_drawable(base_drawable&&) = default;
		auto operator=(base_drawable&&) -> base_drawable& = default;
		inline ~base_drawable() noexcept { free(); }

		base_drawable(window& window, window_specific_material* material
			, gpu_index_buffer* indices
			, Private::base_gpu_vkBuffer_buffer* inputs
			, std::vector<base_gpu_buffer*> fields
		);

	protected: // CompWolf::window_user
		inline void on_window_freeing() noexcept final { free(); }

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !has_window();
		}
		void free() noexcept final;
	};

	namespace Private
	{
		template <typename T>
		struct drawable_field_transform
		{
			using type = gpu_field_buffer<typename T::type>;
		};
		template <auto Index>
		struct drawable_field_transform<type_value_pair<shader_image, Index>>
		{
			using type = gpu_image_buffer;
		};

		template <typename T>
		struct drawable_field_index_transform {};
		template <typename... Ts>
		struct drawable_field_index_transform<Ts...>
		{
			static constexpr std::vector<std::size_t> value = std::vector<std::size_t>(Ts::value...);
		};
	}

	/* An object that is drawn on a window. */
	template <typename MaterialType>
	class drawable : public base_drawable
	{
	public: // type definitions
		/* The type of draw_material used to draw the drawable object. */
		using material_type = MaterialType;
		/* The type of buffer used to contain the drawable object's inputs. */
		using input_buffer_type = gpu_input_buffer<typename material_type::input_shader_type::input_type>;
		/* The type of each buffer used to contain the drawable object's different fields, contained in a type_list. */
		using field_buffer_type = transform_type_list_t<typename material_type::shader_field_types, Private::drawable_field_transform>;

	private: // fields
		material_type* _material;

	public: // accessors
		/* Returns the material used to draw the drawable object. */
		inline auto& material() noexcept { return *_material; }
		/* Returns the material used to draw the drawable object. */
		inline auto& material() const noexcept { return *_material; }

	public: // constructors
		drawable() = default;
		drawable(drawable&&) = default;
		auto operator=(drawable&&) -> drawable& = default;

		template <typename... FieldBufferTypes>
			requires std::same_as<type_list<FieldBufferTypes...>, field_buffer_type>
		drawable(window& window, MaterialType& material
			, gpu_index_buffer& indices
			, input_buffer_type& inputs
			, FieldBufferTypes&... fields
		) : base_drawable(window, &material.vulkan_window_data(window)
			, &indices
			, &inputs
			, std::vector<base_gpu_buffer*>({ static_cast<base_gpu_buffer*>(&fields)... })
		) {}
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAWABLE_HEADER
