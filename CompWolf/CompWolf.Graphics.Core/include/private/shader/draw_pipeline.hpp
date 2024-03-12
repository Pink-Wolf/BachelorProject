#ifndef COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
#define COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER

#include "vulkan_types"
#include "graphics"
#include "window"
#include "shader.hpp"
#include <vector>
#include <owned>
#include <freeable>

namespace CompWolf::Graphics
{
	struct draw_pipeline_settings
	{
		window* target_window;
		shader* vertex_shader;
		shader* fragment_shader;
	};

	class draw_pipeline : public basic_freeable
	{
	private: // fields
		draw_pipeline_settings _settings;

		owned_ptr<Private::vulkan_pipeline_layout> _layout;
		Private::vulkan_render_pass _render_pass;
		Private::vulkan_pipeline _vulkan_pipeline;

		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

	public: // getters
		inline auto target_window() -> window&
		{
			return *_settings.target_window;
		}
		inline auto target_window() const -> const window&
		{
			return *_settings.target_window;
		}

		inline auto vulkan_render_pass() const noexcept
		{
			return _render_pass;
		}
		inline auto vulkan_frame_buffer() const noexcept -> const std::vector<Private::vulkan_frame_buffer>&
		{
			return _frame_buffers;
		}
		inline auto vulkan_pipeline() const noexcept
		{
			return _vulkan_pipeline;
		}

	private: // constructors
		void setup();
	public:
		draw_pipeline() = default;
		draw_pipeline(draw_pipeline&&) = default;
		auto operator =(draw_pipeline&&) -> draw_pipeline& = default;
		inline ~draw_pipeline() noexcept
		{
			free();
		}

		template<typename T>
			requires std::convertible_to<T, draw_pipeline_settings>
		draw_pipeline(T&& settings) : _settings(std::forward<draw_pipeline_settings>(settings))
		{
			setup();
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_layout;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
