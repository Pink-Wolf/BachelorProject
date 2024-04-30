#ifndef COMPWOLF_GRAPHICS_INPUT_MANAGER_HEADER
#define COMPWOLF_GRAPHICS_INPUT_MANAGER_HEADER

#include "vulkan_types"
#include <freeable>
#include "../windows/window_settings.hpp"
#include "key_parameter.hpp"
#include <event>
#include <map>
#include <cctype>

namespace CompWolf
{
	/* Manages inputs from a user, like a keyboard key being pressed.
	 * This should be constructed by the graphics_environment.
	 */
	class input_manager : public basic_freeable
	{
	private: // fields
		bool _not_empty = false;

		std::map<char, event<key_parameter>> _char_pressed;
		event<key_parameter> _any_char_pressed;

		std::map<char, event<key_parameter>> _char_released;
		event<key_parameter> _any_char_released;

	public: // accessors
		/* Gets an event for when the a key representing the given character is beginning to be held down.
		 * This currently supports a-z, A-Z, 0-9, ' '.
		 * This treats uppercase and lowercase as the same character
		 * @throws std::invalid_argument from if this currently does not support the given character.
		 */
		inline auto& char_pressed(char c) { return _char_pressed[static_cast<char>(std::tolower(c))]; }
		/* Gets an event for when any key representing a character is beginning to be held down.
		 * This currently supports a-z, A-Z, 0-9, ' '.
		 */
		inline auto& char_pressed() noexcept { return _any_char_pressed; }
		/* Gets an event for when the a key representing the given character is being released from being held down.
		 * This currently supports a-z, A-Z, 0-9, ' '.
		 * This treats uppercase and lowercase as the same character
		 * @throws std::invalid_argument from if this currently does not support the given character.
		 */
		inline auto& char_released(char c) { return _char_released[static_cast<char>(std::tolower(c))]; }
		/* Gets an event for when any key representing a character is being released from being held down.
		 * This currently supports a-z, A-Z, 0-9, ' '.
		 */
		inline auto& char_released() noexcept { return _any_char_released; }

	public: // vulkan-related
		/* Tells the manager that a key with the given character was pressed. */
		void on_glfw_input(int glfwKey, int glfwAction, int glfwModifiers);

	public: // constructor
		/* Constructs a freed input_manager, as in one that ignores inputs. */
		input_manager() = default;
		input_manager(input_manager&&) = default;
		auto operator=(input_manager&&) -> input_manager& = default;
		inline ~input_manager() noexcept { free(); }

		/* Sets up input-logic for the given window.
		 * @throws std::runtime_error if there was an error while setting up input-logic due to causes outside of the program. */
		inline input_manager(const window_settings&, Private::glfw_window) {}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final { return !_not_empty; }
		inline void free() noexcept final { _not_empty = false; }
	};
}

#endif // ! COMPWOLF_GRAPHICS_INPUT_MANAGER_HEADER
