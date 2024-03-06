#ifndef COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER

#include "graphics"
#include "gpu"
#include "vulkan_types"
#include "window_surface.hpp"
#include <vector>

namespace CompWolf::Graphics
{
	struct swapchain_frame
	{
		Private::vulkan_image_view image = nullptr;
		Private::vulkan_command_pool command_pool = nullptr;

		gpu_fence drawing_fence;
		gpu_semaphore drawing_semaphore;
	};

	/* The actual images of the window' that's surface. */
	class window_swapchain : public basic_gpu_user
	{
	private:
		Private::vulkan_swapchain _vulkan_swapchain = nullptr;
		std::vector<swapchain_frame> _frames;

	public:
		/* Constructs a swapchain that is already destroyed. */
		window_swapchain() = default;
		/* @throws std::runtime_error when something went wrong during window swapchain creation outside of the program. */
		window_swapchain(Private::glfw_window window, window_surface& surface);
		window_swapchain(window_swapchain&&) noexcept;
		auto operator=(window_swapchain&&) noexcept -> window_swapchain&;
		inline ~window_swapchain()
		{
			destroy();
		}

	public:
		/* Whether the swapchain has already been destroyed. */
		inline auto is_destroyed() noexcept -> bool
		{
			return !_vulkan_swapchain;
		}

		/* Destroys the swapchain. */
		void destroy() noexcept;

		inline auto vulkan_swapchain() const noexcept
		{
			return _vulkan_swapchain;
		}

		inline auto frames() noexcept -> std::vector<swapchain_frame>&
		{
			return _frames;
		}
		inline auto frames() const noexcept -> const std::vector<swapchain_frame>&
		{
			return _frames;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER
