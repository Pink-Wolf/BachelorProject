#ifndef COMPWOLF_GRAPHICS_KEY_PRESSED_PARAMETER_HEADER
#define COMPWOLF_GRAPHICS_KEY_PRESSED_PARAMETER_HEADER

#include "key_parameter.hpp"

namespace CompWolf::Graphics
{
	/* Aggregate type used as the parameter for input_manager's events for when a key is pressed. */
	struct key_pressed_parameter : public key_parameter
	{

	};
}

#endif // ! COMPWOLF_GRAPHICS_KEY_PRESSED_PARAMETER_HEADER
