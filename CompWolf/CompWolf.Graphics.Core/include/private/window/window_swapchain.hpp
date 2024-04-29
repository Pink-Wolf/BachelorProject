#ifndef COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER

#include "vulkan_types"
#include "swapchain_frame.hpp"
#include <freeable>
#include <owned>
#include <utility>

namespace CompWolf::Graphics
{
	class gpu_connection;
	class window;

	/* The swapchain of a window, as in the actual images that are being drawn before being displaying on the window. */
	class window_swapchain : basic_freeable
	{
	private: // fields
		owned_ptr<gpu_connection*> _target_gpu;
		Private::vulkan_swapchain _vulkan_swapchain;
		std::vector<swapchain_frame> _frames;
		std::size_t _current_frame_index;

	public: // accessors
		/* Returns the gpu that the window is on. */
		inline auto& gpu() noexcept { return *_target_gpu; }
		/* Returns the gpu that the window is on. */
		inline auto& gpu() const noexcept { return *_target_gpu; }

		/* Returns information about the individual images of the swapchain. */
		inline auto& frames() noexcept { return _frames; }
		/* Returns information about the individual images of the swapchain. */
		inline auto& frames() const noexcept { return _frames; }

		/* Returns the index in frames() that current_frame() is at. */
		inline auto current_frame_index() const noexcept { return _current_frame_index; }

		/* Returns information about the image on the swapchain that is currently being drawn. */
		inline auto& current_frame() noexcept { return frames()[current_frame_index()]; }
		/* Returns information about the image on the swapchain that is currently being drawn. */
		inline auto& current_frame() const noexcept { return frames()[current_frame_index()]; }

	private: // modifiers
		/* Makes the window display the current frame, and makes a new frame the current one. */
		void to_next_frame();

	public: // vulkan-related
		/* Returns the swapchain's vulkan_swapchain, representing a VkSwapchainKHR. */
		inline auto vulkan_swapchain() const noexcept { return _vulkan_swapchain; }


	public: // constructors
		/* Constructs a freed swapchain. */
		window_swapchain() = default;
		window_swapchain(window_swapchain&&) = default;
		auto operator=(window_swapchain&&) -> window_swapchain& = default;

	private:
		window_swapchain(window_settings& args, Private::glfw_window window, window_surface& surface);
		friend window;

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_target_gpu;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER
