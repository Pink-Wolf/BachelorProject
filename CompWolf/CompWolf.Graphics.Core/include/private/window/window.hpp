#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include "window_rebuild_surface_parameters.hpp"
#include "window_settings.hpp"
#include <freeable>
#include <owned>
#include "event"
#include "utility"

namespace CompWolf::Graphics
{
	class graphics_environment;
	class gpu_connection;

	/* A window, as in a rectangle that can be drawn onto, and that listens for various events from outside the program (relating to the window).
	 * This class is thread safe.
	 */
	class window : public basic_freeable
	{
	private: // fields
		window_settings _settings;

		owned_ptr<Private::glfw_window> _glfw_window;
		window_surface _surface;
		window_swapchain _swapchain;

		value_event_wrapper<std::pair<int, int>> _pixel_size;

		enum class draw_event_type
		{
			resize
		};
		std::unordered_map<draw_event_type, std::function<void(window&)>> _draw_events;

	public:
		/* Event invoked before the window's surface is rebuild, for example because the size of the window changed. */
		event<window_rebuild_surface_parameters> rebuilding_surface;
		/* Event invoked after the window's surface is rebuild, for example because the size of the window changed. */
		event<window_rebuild_surface_parameters> rebuild_surface;

	public: // accessors
		/* Returns the gpu that the window is on. */
		inline auto& device() noexcept { return _surface.device(); }
		/* Returns the gpu that the window is on. */
		inline auto& device() const noexcept { return _surface.device(); }

		/* Returns the width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		inline auto& pixel_size() const noexcept { return _pixel_size.const_wrapper(); }

		/* Returns whether the window is not freed. */
		inline auto running() const noexcept { return !empty(); }

	public: // modifiers
		/* Makes the window display what has been drawn onto it (since the last call to update_image). */
		void update_image();
		/* Sets the size of the window, in pixels.
		 * This size does not include any border around the window.
		 * Throws std::runtime_error if there was an error while changing the size due to causes outside of the program.
		 * Throws std::domain_error if given a non-positive width and/or height.
		 */
		void set_pixel_size(int width, int height);

		/* Frees the window. */
		inline void close() noexcept { free(); }

	public: // vulkan-related
		/* Returns the surface of the window, as in the actual area that can display a dynamic image. */
		inline auto& surface() noexcept { return _surface; }
		/* Returns the surface of the window, as in the actual area that can display a dynamic image. */
		inline auto& surface() const noexcept { return _surface; }

		/* Returns the swapchain of the window, as in the actual images that are being drawn before being displaying on the window. */
		inline auto& swapchain() noexcept { return _swapchain; }
		/* Returns the swapchain of the window, as in the actual images that are being drawn before being displaying on the window. */
		inline auto& swapchain() const noexcept { return _swapchain; }

	public: // constructors
		/* Constructs a freed window, as in one that is already closed. */
		window() = default;
		window(window&&) = default;
		auto operator=(window&&) -> window& = default;
		inline ~window() noexcept { free(); }

	private:
		window(graphics_environment*, gpu_connection*, window_settings);
	public:

		/* Constructs a window with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument from (2) and (3) if the given settings have invalid settings.
		 */
		inline window(graphics_environment& environment, window_settings settings) : window(&environment, nullptr, settings) {}
		/* Constructs a window on the given gpu, with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument from (2) and (3) if the given settings have invalid settings.
		 */
		inline window(gpu_connection& gpu, window_settings settings) : window(nullptr, &gpu, settings) {}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_glfw_window;
		}
		void free() noexcept final;

		/* Invoked right before the window's data is freed.
		 * @see free()
		 */
		event<> freeing;
		/* Invoked right after the window's data is freed.
		 * @see free()
		 */
		event<> freed;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_HEADER
