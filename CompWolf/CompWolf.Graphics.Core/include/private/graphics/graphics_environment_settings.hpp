#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER

#include <functional>
#include <string>
#include <version_number>

namespace CompWolf::Graphics
{
	/* Aggregate type used by graphics_environment's constructor to specify program-wide behaviour. */
	struct graphics_environment_settings
	{
		/* What the graphics_environment and related objects should report as the name of the program.
		 * This does NOT need to match the name specified in other places, like the name of the .exe.
		 */
		std::string program_name = "Compwolf Program";
		/* What the graphics_environment and related objects should report as the version of the program.
		 * This does NOT need to match the version specified in other places.
		 */
		version_number program_version = { 0, 0, 0 };

		/* Should almost always be left empty.
		 * If non-empty, sets up internal debugging-logic used to debug CompWolf.Graphics.Core itself; Debug-messages are then passed to this function. Debug-messages ends with a newline character (and finally \0).
		 */
		std::function<void(std::string)> internal_debug_callback;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER
