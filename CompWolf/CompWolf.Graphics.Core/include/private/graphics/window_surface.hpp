#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER

#include "graphics_environment.hpp"
#include "vulkan_types"
#include "graphics_environment/gpu_thread.hpp"
#include <vector>

namespace CompWolf::Graphics
{
	/* The part of a window one can draw on. */
	class window_surface
	{
	private:
		graphics_environment* _environment;

		Private::vulkan_surface _vulkan_surface;
		Private::vulkan_swapchain _swapchain;
		std::vector<Private::vulkan_image_view> _swapchain_images;

		persistent_job_key _draw_present_job;

	public:
		/* Constructs a surface that is already destroyed. */
		window_surface() = default;
		/* @throws std::runtime_error when something went wrong during window surface creation outside of the program. */
		window_surface(graphics_environment& environment, Private::glfw_window& window);
		window_surface(window_surface&&) noexcept;
		auto operator=(window_surface&&) noexcept -> window_surface&;
		~window_surface();

	public:
		/* Whether the surface has already been destroyed. */
		inline auto is_destroyed() noexcept -> bool
		{
			return !_vulkan_surface;
		}

		/* Destroys the surface. */
		void destroy() noexcept;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
