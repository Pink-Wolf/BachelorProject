#ifndef COMPWOLF_GRAPHICS_WINDOW_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_HEADER

#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include "window_rebuild_surface_parameters.hpp"
#include "draw_code_parameters.hpp"
#include "window_settings.hpp"
#include <freeable>
#include <owned>
#include "event"
#include "utility"
#include "gpu_programs"
#include <functional>
#include "inputs"

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

		std::vector<gpu_program> _frame_drawing_programs;
		event<draw_code_parameters> _drawing_code;

		value_event_wrapper<std::pair<int, int>> _pixel_size;

		enum class draw_event_type
		{
			resize
		};
		std::unordered_map<draw_event_type, std::function<void(window&)>> _draw_events;

		input_manager _inputs;

	public:
		/* Event invoked before the window's surface is rebuild, for example because the size of the window changed. */
		event<window_rebuild_surface_parameters> rebuilding_surface;
		/* Event invoked after the window's surface is rebuild, for example because the size of the window changed. */
		event<window_rebuild_surface_parameters> rebuild_surface;

	private: // event handlers
		void frame_drawing_program_code(std::size_t frame_index, const gpu_program_code_parameters&);

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

		/* Returns the input_manager handling inputs while the window is focused */
		inline auto& inputs() noexcept { return _inputs; }
		/* Returns the input_manager handling inputs while the window is focused */
		inline auto& inputs() const noexcept { return _inputs; }

	public: // modifiers
		/* Makes the window update what is shown on it.
		 * The window updates its image by making its swapchain's current frame visible. It then changes what is its current frame.
		 */
		void update_image();

		/* The key used to identify some drawing-code added to a window with add_draw_code. */
		using draw_code_key = event<draw_code_parameters>::key_type;
		/* Adds the given code to be run when the window's image is being updated.
		 * Returns a key used to identify the piece of code.
		 */
		inline auto add_draw_code(std::function<void(draw_code_parameters&)> code) -> draw_code_key
		{
			_frame_drawing_programs.clear();
			return _drawing_code.subscribe([code](const event<draw_code_parameters>&, draw_code_parameters& args) { code(args); });
		}
		/* Removes the given code from being run when the window's image is being updated. */
		inline void remove_draw_code(draw_code_key code) noexcept
		{
			_frame_drawing_programs.clear();
			return _drawing_code.unsubscribe(code);
		}

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
