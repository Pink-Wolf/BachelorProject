#ifndef COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER

#include <string_view>
#include <utility>

namespace CompWolf
{
	/* Aggregate type used by window's constructor to specify its behaviour. */
	struct window_settings
	{
		/* The name of the window.
		 * If the window has a border, then this may for example be displayed on the border.
		 */
		std::string_view name;
		/* The initial width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		std::pair<int, int> pixel_size;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER
