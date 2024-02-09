#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "graphics_environment.h"
#include "private/vulkan_types.h"
#include <string>
#include <event.h>

namespace CompWolf::Graphics
{
	struct window_close_parameter
	{

	};

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window). */
	class window
	{
	public:
		window(const graphics_environment& environment);
		window(window&&) noexcept;
		auto operator=(window&&) noexcept -> window&;
		~window();

	public:
		/* Whether the window is currently open. */
		auto is_open() noexcept -> bool;

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

	private:
		Private::glfw_window* glfw_window;
	};

	auto inline window::is_open() noexcept -> bool
		{ return glfw_window != nullptr; }
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
