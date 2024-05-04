#ifndef COMPWOLF_GRAPHICS_WINDOW_USER_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_USER_HEADER

#include <event>
#include "window_rebuild_surface_parameters.hpp"

namespace CompWolf
{
	class window;

	/* Abstract type containing a reference to a window.
	 * If the window is freed, then this is freed first.
	 * Also contains virtual methods for events related to the window, like when its surface is rebuild.
	 */
	class window_user
	{
	private: // fields
		window* _target_window = nullptr;
		event<void>::key_type _freeing_event_key;
		event<window_rebuild_surface_parameters&>::key_type _rebuilding_event_key;

	public: // accessors
		/* The window that the object targets. */
		inline auto& target_window() noexcept { return *_target_window; }
		/* The window that the object targets. */
		inline auto& target_window() const noexcept { return *_target_window; }
	protected:
		/* Whether the object is actually referencing a window. */
		inline auto has_window() const noexcept { return !!_target_window; }

	protected: // modifiers
		/* Sets what window the object is referencing. */
		void set_window(window*) noexcept;

	protected: // virtual
		/* Invoked after the target window's surface is rebuild. */
		inline virtual void on_rebuild_surface(window_rebuild_surface_parameters&) {}

		/* Invoked before the target window is being freed. */
		inline virtual void on_window_freeing() noexcept {}

	public: // constructors
		/* Constructs a window_user that does not point to any window. */
		window_user() = default;
		inline window_user(window_user& o) noexcept { set_window(&o.target_window()); }
		inline auto operator=(window_user& o) noexcept -> window_user& { set_window(&o.target_window()); return *this; }
		inline window_user(window_user&& o) noexcept
		{
			set_window(&o.target_window());
			o.set_window(nullptr);
		}
		inline auto operator=(window_user&& o) noexcept -> window_user&
		{
			set_window(&o.target_window());
			o.set_window(nullptr);
			return *this;
		}
		inline ~window_user() noexcept { set_window(nullptr); }

		/* Constructs a window_user with a reference to the given window. */
		inline window_user(window& w) noexcept { set_window(&w); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_USER_HEADER
