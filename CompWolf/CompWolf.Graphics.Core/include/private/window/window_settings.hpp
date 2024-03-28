#ifndef COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER

#include <string_view>
#include <utility>

namespace CompWolf::Graphics
{
	struct window_settings
	{
		std::string_view name;
		std::pair<int, int> pixel_size;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SETTINGS_HEADER
