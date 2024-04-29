#ifndef COMPWOLF_GRAPHICS_KEY_PARAMETER_HEADER
#define COMPWOLF_GRAPHICS_KEY_PARAMETER_HEADER

namespace CompWolf::Graphics
{
	/* Aggregate type used as the parameter for input_manager's events for when a key is pressed, released, etc. */
	struct key_parameter
	{
		/* If the key represents a character, the character.
		 * The character is uppercase if caps lock is active or a shift-key is held down (but not both); the character is otherwise lowercase.
		 */
		char character;
		/* If the key represents a character, the character.
		 * The character is lowercase.
		 */
		char lowercase_character;
		/* If the key represents a character, the character.
		 * The character is uppercase.
		 */
		char uppercase_character;
	};
}

#endif // ! COMPWOLF_GRAPHICS_KEY_PARAMETER_HEADER
