#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER

#include "vulkan_types"
#include <string>
#include <functional>
#include <version_number>
#include <vector>

namespace CompWolf::Graphics
{
	/* @see graphics_environment */
	struct graphics_environment_settings
	{
		/* The type of function that internal_debug_callback is.
		 * As in, when internal_debug_callback is some type std::function<T>, then this is the T.
		 */
		using internal_debug_callback_function_type = void(std::string);
		/* If non-empty, sets up extra internal debugging-logic for CompWOlf::Graphics and passes messages to this function.
		 * The message ends with a newline character.
		 */
		std::function<internal_debug_callback_function_type> internal_debug_callback;

		/* The name of the program. */
		std::string program_name = "Compwolf Program";
		/* The version of the program. */
		version_number program_version = { 0, 0, 0 };
	};
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_SETTINGS_HEADER
