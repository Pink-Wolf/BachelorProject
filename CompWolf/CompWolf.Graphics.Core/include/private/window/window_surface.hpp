#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER

#include "vulkan_types"
#include "gpu"
#include "graphics"
#include <vector>

namespace CompWolf::Graphics
{
	/* The part of a window one can draw on. */
	class window_surface : public basic_gpu_user
	{
	private:
		Private::vulkan_surface _vulkan_surface = nullptr;

		Private::surface_format_handle _format;

		gpu_job _draw_present_job;

	public:
		/* Constructs a surface that is already destroyed. */
		window_surface() = default;
		/* @throws std::runtime_error when something went wrong during window surface creation outside of the program. */
		window_surface(graphics_environment& environment, Private::glfw_window& window);
		window_surface(window_surface&&) noexcept;
		auto operator=(window_surface&&) noexcept -> window_surface&;
		~window_surface();

		window_surface(const window_surface&) = delete;
		auto operator=(const window_surface&) -> window_surface& = delete;

	public:
		/* Whether the surface has already been destroyed. */
		inline auto is_destroyed() noexcept -> bool
		{
			return !_vulkan_surface;
		}

		/* Destroys the surface. */
		void destroy() noexcept;

		inline auto surface() const noexcept
		{
			return _vulkan_surface;
		}

		inline auto format() const noexcept -> Private::const_surface_format_handle
		{
			return _format;
		}

		inline auto draw_present_job() noexcept -> gpu_job&
		{
			return _draw_present_job;
		}
		inline auto draw_present_job() const noexcept -> const gpu_job&
		{
			return _draw_present_job;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
