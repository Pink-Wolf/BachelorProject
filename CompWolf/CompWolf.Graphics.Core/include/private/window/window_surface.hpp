#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER

#include "vulkan_types"
#include "gpu"
#include "graphics"
#include <vector>
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	struct window_surface_settings
	{
		// What device the surface should be on, or null if it may be on any device.
		gpu* target_device;
	};

	/* The part of a window one can draw on. */
	class window_surface : public basic_freeable
	{
	private: // fields
		owned_ptr<gpu*> _target_gpu;
		Private::vulkan_surface _vulkan_surface;
		Private::surface_format_handle _format;
		gpu_job _draw_present_job;

	public: // getters
		inline auto device() noexcept -> gpu&
		{
			return *_target_gpu;
		}
		inline auto device() const noexcept -> const gpu&
		{
			return *_target_gpu;
		}

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

	public: // constructor
		/* Constructs a surface that is already freed. */
		window_surface() = default;
		window_surface(window_surface&&) = default;
		auto operator=(window_surface&&) -> window_surface& = default;
		inline ~window_surface() noexcept
		{
			free();
		}

		/* @throws std::runtime_error when something went wrong during window surface creation outside of the program. */
		window_surface(graphics_environment& environment, Private::glfw_window& window, window_surface_settings settings);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_target_gpu;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
