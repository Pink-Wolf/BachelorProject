#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "gpus"
#include "graphics"
#include "vulkan_types"
#include "window_settings.hpp"
#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include <event>
#include <value_mutex>
#include <utility>
#include <freeable>
#include <owned>
#include <string_view>
#include <concepts>

namespace CompWolf::Graphics
{
	struct window_close_parameter
	{

	};

	struct window_rebuild_swapchain_parameter
	{
		window_surface* old_surface;
		window_surface* new_surface;

		window_swapchain* old_swapchain;
		window_swapchain* new_swapchain;
	};

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window).
	 * This class is thread safe.
	 */
	class window : public basic_freeable
	{
	private: // fields
		graphics_environment* _environment;
		window_settings _settings;

		using glfw_window_type = owned_ptr<Private::glfw_window>;
		glfw_window_type _glfw_window;

		window_surface _surface;
		window_swapchain _swapchain;

		value_event_wrapper<std::pair<int, int>> _pixel_size;

		enum class draw_event_type
		{
			resize
		};
		std::unordered_map<draw_event_type, std::function<void(window&)>> _draw_events;

	public:
		event<window_rebuild_swapchain_parameter> swapchain_rebuilding;
		event<window_rebuild_swapchain_parameter> swapchain_rebuilded;

	public: // getters

		/* Whether the window is currently open. */
		inline auto is_open() const noexcept -> bool
		{
			return !empty();
		}

		auto environment() noexcept -> graphics_environment&
		{
			return *_environment;
		}
		auto environment() const noexcept -> const graphics_environment&
		{
			return *_environment;
		}

		auto device() noexcept -> gpu_connection&
		{
			return _surface.device();
		}
		auto device() const noexcept -> const gpu_connection&
		{
			return _surface.device();
		}

		inline auto surface() noexcept -> window_surface&
		{
			return _surface;
		}
		inline auto surface() const noexcept -> const window_surface&
		{
			return _surface;
		}

		inline auto swapchain() noexcept -> window_swapchain&
		{
			return _swapchain;
		}
		inline auto swapchain() const noexcept -> const window_swapchain&
		{
			return _swapchain;
		}

		inline auto draw_present_job() noexcept -> gpu_job&
		{
			return surface().draw_present_job();
		}
		inline auto draw_present_job() const noexcept -> const gpu_job&
		{
			return surface().draw_present_job();
		}

		inline auto pixel_size() const -> const_value_event_wrapper<std::pair<int, int>>&
		{
			return _pixel_size.const_wrapper();
		}

	private: // setters
		void set_pixel_size(int width, int height);

	public: // other methods
		void update_image();

		/* Closes the window. */
		inline void close() noexcept
		{
			free();
		}
		/* Invoked right before the window closes.
		 * @see close()
		 */
		event<window_close_parameter> closing;
		using closing_parameter_type = window_close_parameter;
		/* Invoked right after the window closes.
		 * @see close()
		 */
		event<window_close_parameter> closed;
		using closed_parameter_type = window_close_parameter;

	private: // constructors
		void setup();
	public:
		/* Constructs a window that is already closed. */
		window() = default;
		window(window&&) = default;
		auto operator=(window&&) -> window& = default;
		inline ~window() noexcept
		{
			free();
		}

		/* @throws std::runtime_error when something went wrong during window creation outside of the program. */
		template <typename SettingsInputType>
			requires std::constructible_from<window_settings, SettingsInputType>
		window(graphics_environment& environment, SettingsInputType settings)
			: _environment(&environment), _settings(settings)
		{
			setup();
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_glfw_window;
		}
		void free() noexcept final;

		/* Invoked right before the window's data is freed.
		 * @see free()
		 */
		event<> freeing;
		/* Invoked right after the window's data is freed.
		 * @see free()
		 */
		event<> freed;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
