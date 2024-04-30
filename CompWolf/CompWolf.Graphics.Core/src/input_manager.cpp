#include "pch.h"
#include "inputs"

#include "compwolf_vulkan.hpp"
#include "window"

namespace CompWolf
{
	/******************************** vulkan-related ********************************/

	void input_manager::on_glfw_input(int glfwKey, int glfwAction, int glfwModifiers)
	{
		bool capitalized
			= ((glfwModifiers & GLFW_MOD_SHIFT) != 0)
			!= ((glfwModifiers & GLFW_MOD_CAPS_LOCK) != 0);

		{
			char character = 0;
			if (glfwKey == 32) character = ' ';
			else if (48 <= glfwKey && glfwKey <= 57) character = '0' + (glfwKey - 48);
			else if (65 <= glfwKey && glfwKey <= 90) character = 'a' + (glfwKey - 65);
			if (character != 0)
			{
				key_parameter key_args{
					.lowercase_character = character,
					.uppercase_character = static_cast<char>(std::toupper(character)),
				};
				key_args.character = capitalized ? key_args.uppercase_character : key_args.lowercase_character;

				switch (glfwAction)
				{
				case GLFW_PRESS:
				{
					_any_char_pressed.invoke(key_args);

					auto char_pressed_iterator = _char_pressed.find(key_args.lowercase_character);
					if (char_pressed_iterator != _char_pressed.end()) char_pressed_iterator->second.invoke(key_args);
				} break;
				case GLFW_RELEASE:
				{
					_any_char_released.invoke(key_args);

					auto char_released_iterator = _char_released.find(key_args.lowercase_character);
					if (char_released_iterator != _char_released.end()) char_released_iterator->second.invoke(key_args);
				} break;
				default: break;
				}
			}
		}
	}
}
