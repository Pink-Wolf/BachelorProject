#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "graphics"
#include "vulkan_types"
#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include <event>
#include <value_mutex>
#include <utility>

namespace CompWolf::Graphics
{
	struct window_close_parameter
	{

	};

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window).
	 * This class is thread safe.
	 */
	class window : public gpu_user
	{
	private:
		using glfw_window_type = shared_value_mutex<Private::glfw_window>;
		glfw_window_type _glfw_window;

		window_surface _surface;
		window_swapchain _swapchain;

		value_event_wrapper<std::pair<int, int>> _pixel_size;

	public:
		/* Constructs a window that is already closed. */
		window() = default;
		/* @throws std::runtime_error when something went wrong during window creation outside of the program. */
		window(graphics_environment& environment);
		window(window&&) noexcept;
		auto operator=(window&&) noexcept -> window&;
		~window();

	public:
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

		inline auto pixel_size() -> const_value_event_wrapper<std::pair<int, int>>&
		{
			return _pixel_size.const_wrapper();
		}

	public:
		/* Whether the window is currently open. */
		inline auto is_open() noexcept -> bool
		{
			return _glfw_window.get_value_copy_quick() != nullptr;
		}

		/* Closes the window. */
		void close() noexcept;

		using closing_type = event<window_close_parameter>;
		/* Invoked right before the window closes.
		 * @see close()
		 */
		closing_type closing;
		using closed_type = event<window_close_parameter>;
		/* Invoked right after the window closes.
		 * @see close()
		 */
		closed_type closed;

	public:
		inline auto device() -> gpu& final
		{
			return _surface.device();
		}
		inline auto device() const -> const gpu& final { return gpu_user::device(); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
