#ifndef COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
#define COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER

#include "vulkan_types"
#include "graphics"
#include "window"
#include "shader.hpp"
#include <vector>

namespace CompWolf::Graphics
{
	struct draw_pipeline_settings
	{
		window* target_window;
		shader* vertex_shader;
		shader* fragment_shader;
	};

	class draw_pipeline : window_user
	{
	private:
		draw_pipeline_settings _settings;

		Private::vulkan_pipeline_layout _layout = nullptr;
		Private::vulkan_render_pass _render_pass;
		Private::vulkan_pipeline _vulkan_pipeline;

		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

	private:
		void setup();
	public:
		void clear() noexcept;
	public:
		draw_pipeline() = default;

		template<typename T>
			requires std::convertible_to<T, draw_pipeline_settings>
		draw_pipeline(T settings) : _settings(std::forward<draw_pipeline_settings>(settings))
		{
			setup();
		}

		draw_pipeline(draw_pipeline&&) noexcept;
		auto operator =(draw_pipeline&&) noexcept -> draw_pipeline&;
		draw_pipeline(const draw_pipeline&) noexcept = delete;
		auto operator =(const draw_pipeline&) noexcept -> draw_pipeline & = delete;

		~draw_pipeline()
		{
			clear();
		}

	public:
		inline auto empty() const noexcept -> bool
		{
			return !_layout;
		}

	public:
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

	public:
		inline auto target_window() -> window& final
		{
			return *_settings.target_window;
		}
		inline auto target_window() const -> const window& final { return window_user::target_window(); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
