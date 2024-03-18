#include "pch.h"
#include "window"
#include <functional>

namespace CompWolf::Graphics
{
	/******************************** setters ********************************/

	void window_user::set_target_window(window* w) noexcept
	{
		if (has_window())
			target_window().swapchain_rebuilded.unsubscribe(_swapchain_rebuilded_key);

		set_dependent(w);

		if (has_window())
			_swapchain_rebuilded_key = target_window().swapchain_rebuilded.subscribe
			(
				std::bind_front(&window_user::on_swapchain_rebuild, this)
			);
	}
}
