#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "graphics_environment.hpp"
#include "vulkan_types"
#include <string>
#include <event>
#include <value_mutex>
#include "graphics_environment/gpu_thread.hpp"

namespace CompWolf::Graphics
{
	struct window_close_parameter
	{

	};

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window).
	 * This class is thread safe.
	 */
	class window
	{
	private:
		graphics_environment* _environment;

		using glfw_window_type = shared_value_mutex<Private::glfw_window>;
		glfw_window_type _glfw_window;
		Private::vulkan_surface _vulkan_surface;
		Private::vulkan_swapchain _swapchain;

		persistent_job_key _draw_present_job;

	public:
		/* @throws std::runtime_error when something went wrong during window creation outside of the program. */
		window(graphics_environment& environment);
		window(window&&) noexcept;
		auto operator=(window&&) noexcept -> window&;
		~window();

	public:
		/* Whether the window is currently open. */
		inline auto is_open() noexcept -> bool
		{
			return _glfw_window.get_value_copy_quick() != nullptr;
		}

		/* Closes the window. */
		void close() noexcept;
		/* Invoked right before the window closes.
		 * @see close()
		 */
		event<window_close_parameter> closing;
		/* Invoked right after the window closes.
		 * @see close()
		 */
		event<window_close_parameter> closed;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
