#ifndef  COMPWOLF_GRAPHICS_CORE_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_CORE_WINDOW_HEADER

#include "graphics_environment.h"
#include "glfw_types.h"
#include <string>

namespace CompWolf::Graphics::Core
{
	class window
	{
	public:
		window(const graphics_environment* environment);
		window(window&&) noexcept;
		auto operator=(window&&) noexcept -> window&;
		~window();

	public:
		auto is_open() noexcept -> bool;

		static void update();
		void close() noexcept;

	private:
		Private::glfw_window* glfw_window;
	};

	auto inline window::is_open() noexcept -> bool
		{ return glfw_window != nullptr; }
}

#endif // ! COMPWOLF_GRAPHICS_CORE_WINDOW_HEADER
