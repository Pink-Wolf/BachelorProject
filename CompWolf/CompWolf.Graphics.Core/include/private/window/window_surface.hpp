#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_connection;
	class graphics_environment;
	class window;

	/* The surface of a window, as in the actual area that can display a dynamic image. */
	class window_surface : public basic_freeable
	{
	private: // fields
		owned_ptr<gpu_connection*> _target_gpu;
		Private::vulkan_surface _vulkan_surface;
		Private::surface_format_handle _format;
		Private::vulkan_render_pass _render_pass;

	public: // accessors
		/* Returns the gpu that the window is on. */
		inline auto& gpu() noexcept { return *_target_gpu; }
		/* Returns the gpu that the window is on. */
		inline auto& gpu() const noexcept { return *_target_gpu; }

	public: // vulkan-related

		/* Returns the surface's vulkan_surface, representing a VkSurfaceKHR. */
		inline auto vulkan_surface() const noexcept { return _vulkan_surface; }
		/* Returns the surface's surface_format_handle, representing a surface_format_info*. */
		inline auto vulkan_format() const noexcept { return _format; }
		/* Returns the surface's vulkan_render_pass, representing a VkRenderPass. */
		inline auto vulkan_render_pass() const noexcept { return _render_pass; }

	public: // constructor
		/* Constructs a freed surface. */
		window_surface() = default;
		window_surface(window_surface&&) = default;
		auto operator=(window_surface&&) -> window_surface& = default;
		inline ~window_surface() noexcept { free(); }

	private:
		window_surface(window_settings&, Private::glfw_window&, graphics_environment* optional_environment, gpu_connection* optional_gpu);
		friend window;

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_target_gpu;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE_HEADER
