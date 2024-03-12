#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include <gpu>
#include "graphics"
#include "vulkan_types"
#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include <event>
#include <value_mutex>
#include <utility>
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	struct window_close_parameter
	{

	};
	struct window_free_parameter
	{

	};

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window).
	 * This class is thread safe.
	 */
	class window : public basic_freeable
	{
	private: // fields
		using glfw_window_type = owned_ptr<Private::glfw_window>;
		glfw_window_type _glfw_window;

		window_surface _surface;
		window_swapchain _swapchain;

		value_event_wrapper<std::pair<int, int>> _pixel_size;

	public: // getters

		/* Whether the window is currently open. */
		inline auto is_open() const noexcept -> bool
		{
			return !empty();
		}

		auto device() noexcept -> gpu&
		{
			return _surface.device();
		}
		auto device() const noexcept -> const gpu&
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

	public: // other methods

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

	public: // constructors
		/* Constructs a window that is already closed. */
		window() = default;
		window(window&&) = default;
		auto operator=(window&&) -> window& = default;
		inline ~window() noexcept
		{
			free();
		}

		/* @throws std::runtime_error when something went wrong during window creation outside of the program. */
		window(graphics_environment& environment);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_glfw_window;
		}
		void free() noexcept final;

		/* Invoked right before the window's data is freed.
		 * @see free()
		 */
		event<window_free_parameter> freeing;
		using freeing_parameter_type = window_free_parameter;
		/* Invoked right after the window's data is freed.
		 * @see free()
		 */
		event<window_free_parameter> freed;
		using freed_parameter_type = window_free_parameter;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
