#include "pch.h"
#include "window"
#include <functional>

namespace CompWolf::Graphics
{
	/******************************** modifiers ********************************/

	void window_user::set_window(window* new_window) noexcept
	{
		if (has_window())
		{
			target_window().freeing.unsubscribe(_freeing_event_key);
			target_window().rebuild_surface.unsubscribe(_rebuilding_event_key);
		}

		_target_window = new_window;

		if (has_window())
		{
			_freeing_event_key = target_window().freeing.subscribe([this]()
				{
					free();
					set_window(nullptr);
				}
			);
			_rebuilding_event_key = target_window().rebuild_surface.subscribe
			(
				std::bind_front(&window_user::on_rebuild_surface, this)
			);
		}
	}
}
