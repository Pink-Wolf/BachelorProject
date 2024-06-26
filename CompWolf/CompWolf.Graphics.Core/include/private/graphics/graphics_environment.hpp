#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER

#include "vulkan_types"
#include <thread>
#include <concepts>
#include <event>
#include <freeable>

#include "graphics_environment_settings.hpp"
#include "glfw_handle.hpp"
#include "vulkan_handle.hpp"
#include "vulkan_debug_handle.hpp"
#include "gpus"

namespace CompWolf
{
	struct graphics_environment_update_parameter
	{

	};

	/* Sets up and manages program-wide logic used for connecting to the gpus.
	 * The environment's update-method should normally be called continually.
	 * The thread that constructed the graphics_environment is considered the "main graphics thread".
	 */
	class graphics_environment : public basic_freeable
	{
	private: // fields
		/* Whether an instance of the environment has already been constructed, which has not yet been destructed. */
		static bool _constructed;
		/* The settings for the environment. */
		graphics_environment_settings _settings;
		/* The main thread for handling graphics and windows.
		 * This is the thread the main function was called by.
		 */
		std::thread::id _main_graphics_thread;

		// The order of the following data members is important for their destructors to be called in the right order.
		/* Contains program-wide logic for GLFW. */
		glfw_handle _glfw_handle;
		/* Contains program-wide logic for Vulkan. */
		vulkan_handle _vulkan_handle;
		/* Contains logic for getting debug messenges from Vulkan. */
		vulkan_debug_handle _vulkan_debug_handle;
		/* Connections to the various gpus on the machine. */
		gpu_manager _gpus;

	public: // accessors
		/* Returns the individual connections to each gpu. */
		inline auto& gpus() noexcept { return _gpus.gpus(); }
		/* Returns the individual connections to each gpu. */
		inline auto& gpus() const noexcept { return _gpus.gpus(); }

		/* Returns whether the thread with the given id is the main graphics thread. */
		inline auto is_main_thread(std::thread::id id) noexcept -> bool
		{
			return _main_graphics_thread == id;
		}
		/* Returns whether the thread calling this is the main graphics thread. */
		inline auto is_this_main_thread() noexcept -> bool
		{
			return is_main_thread(std::this_thread::get_id());
		}

		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf.
		 * Returns the vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance vulkan_instance() const noexcept
		{
			return _vulkan_handle.vulkan_instance();
		}

	public: // other methods
		/* Handles events received from outside the program.
		 * @throws std::logic_error if not run by the main graphics thread.
		 */
		void update();
		/* Event invoked before any other logic in the environment's update-method. */
		event<graphics_environment_update_parameter&> updating;
		/* Event invoked after all other logic in the environment's update-method. */
		event<graphics_environment_update_parameter&> updated;

	public: // constructor
		/* Constructs a freed environment, as in one without any logic.
		 * This does not count as an environment that exists.
		 */
		graphics_environment() = default;
		graphics_environment(graphics_environment&&) = default;
		auto operator=(graphics_environment&&) -> graphics_environment& = default;
		/* Must be called from the main graphics thread, or else is undefined behaviour. */
		inline ~graphics_environment() noexcept { free(); }

		/* Constructs a valid environment, and sets up program-wide logic used for connecting to the gpus, based on the given settings.
		 * @throws std::logic_error if an environment already exists (which has not been freed).
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		graphics_environment(graphics_environment_settings);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _glfw_handle.empty();
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
