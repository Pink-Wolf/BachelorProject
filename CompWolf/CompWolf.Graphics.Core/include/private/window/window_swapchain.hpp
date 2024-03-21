#ifndef COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER

#include "graphics"
#include "gpu"
#include "vulkan_types"
#include "window_surface.hpp"
#include <vector>
#include <freeable>
#include <owned>
#include <utility>

namespace CompWolf::Graphics
{
	struct swapchain_frame
	{
		Private::vulkan_image_view image;
		Private::vulkan_command_pool command_pool;
		
		std::vector<std::pair<gpu_fence, gpu_semaphore>> synchronizations;

	public: // getter
		inline auto last_vulkan_semaphore() -> Private::vulkan_semaphore
		{
			if (synchronizations.empty()) return nullptr;
			return synchronizations.back().second.vulkan_semaphore();
		}
	};

	/* The actual images of the window' that's surface. */
	class window_swapchain : basic_freeable
	{
	private: // fields
		owned_ptr<gpu*> _target_gpu;
		Private::vulkan_swapchain _vulkan_swapchain;
		std::vector<swapchain_frame> _frames;
		size_t _current_frame_index;

	public: // getters
		inline auto device() noexcept -> gpu&
		{
			return *_target_gpu;
		}
		inline auto device() const noexcept -> const gpu&
		{
			return *_target_gpu;
		}

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

		inline auto current_frame() noexcept -> swapchain_frame&
		{
			return frames()[_current_frame_index];
		}
		inline auto current_frame() const noexcept -> const swapchain_frame&
		{
			return frames()[_current_frame_index];
		}
		inline auto current_frame_index() const noexcept { return _current_frame_index; }

	public: // other methods
		void to_next_frame();

	public: // constructors
		/* Constructs a swapchain that is already destroyed. */
		window_swapchain() = default;
		window_swapchain(window_swapchain&&) = default;
		auto operator=(window_swapchain&&) -> window_swapchain& = default;

		/* @throws std::runtime_error when something went wrong during window swapchain creation outside of the program. */
		window_swapchain(Private::glfw_window window, window_surface& surface);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_target_gpu;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN_HEADER