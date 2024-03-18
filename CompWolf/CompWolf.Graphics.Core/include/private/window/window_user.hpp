#ifndef COMPWOLF_GRAPHICS_WINDOW_USER_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_USER_HEADER

#include "window.hpp"
#include <free_on_dependent_freed>

namespace CompWolf::Graphics
{
	class window_user : private free_on_dependent_freed<window>
	{
	private: // fields
		event<window_rebuild_swapchain_parameter>::key_type _swapchain_rebuilded_key;

	protected: // event handlers
		virtual void on_swapchain_rebuild(
			const event<window_rebuild_swapchain_parameter>&,
			window_rebuild_swapchain_parameter&
		) = 0;

	public: // getters
		inline auto target_window() noexcept -> window&
		{
			window* p;
			get_dependent(p);
			return *p;
		}
		inline auto target_window() const noexcept -> const window&
		{
			const window* p;
			get_dependent(p);
			return *p;
		}
	protected:
		inline auto has_window() const noexcept
		{
			return is_subscribed_to_dependent();
		}

	protected: // setters
		void set_target_window(window*) noexcept;
		inline void set_target_window(window& w) noexcept
		{
			set_target_window(&w);
		}

	public: // constructors
		window_user() = default;
		window_user(window_user& other) noexcept
		{
			set_target_window(other.target_window());
		}
		auto operator=(window_user& other) noexcept -> window_user&
		{
			set_target_window(other.target_window());
		}
		window_user(window_user&& other) noexcept
		{
			set_target_window(other.target_window());
			other.set_target_window(nullptr);
		}
		auto operator=(window_user&& other) noexcept -> window_user&
		{
			set_target_window(other.target_window());
			other.set_target_window(nullptr);
		}
		~window_user() noexcept
		{
			set_target_window(nullptr);
		}

		window_user(window& dependent) noexcept
		{
			set_target_window(&dependent);
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_USER_HEADER
