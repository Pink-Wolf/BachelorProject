#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER

#include "vulkan_types"
#include "graphics_environment/graphics_environment_settings.hpp"
#include "graphics_environment/glfw_handle.hpp"
#include "graphics_environment/vulkan_handle.hpp"
#include "graphics_environment/vulkan_debug_handle.hpp"
#include "graphics_environment/gpu_manager.hpp"
#include <string>
#include <thread>
#include <vector>
#include <event>
#include <empty_pointer>
#include <memory>
#include <version_number>
#include <atomic>

namespace CompWolf::Graphics
{
	struct graphics_environment_update_parameter
	{

	};

	/* Manages program-wide logic used to handle graphics and windows.
	 * Only a single instance of graphics_environment may exist at once in a program.
	 * This class is thread safe, except its constructor and destructor which must run on the main graphics thread.
	 */
	class graphics_environment
	{
	private:
		/* Whether an instance of the environment has already been constructed, which has not yet been destructed. */
		static std::atomic_bool _constructed;

		/* The settings for the environment. */
		graphics_environment_settings _settings;

		/* The main thread for handling graphics and windows.
		 * This is the thread the main function was called by.
		 */
		std::thread::id _main_graphics_thread;

		/* --------------------------------------- The order of the below items is important, so they are destructed in the right order! --------------------------------------- */
		// The order of the following data members is important for their destructors to be called in the right order.
		/* Contains program-wide logic for GLFW. */
		glfw_handle _glfw_handle;
		/* Contains program-wide logic for Vulkan. */
		vulkan_handle _vulkan_handle;
		/* Contains logic for getting debug messenges from Vulkan. */
		vulkan_debug_handle _vulkan_debug_handle;
		/* Connections to the various gpus on the machine. */
		gpu_manager _gpus;

	public:
		/* If not yet set up, sets up program-wide logic.
		 * If not run on the thread running the main function, then this will have undefined behaviour.
		 * @param A graphics_environment_settings specifying how the environment should behave.
		 * @typeparam SettingsInputType The type of graphics_environment_settings to pass onto the environment.
		 * @throws std::logic_error when an instance of graphics_environment already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		template <typename SettingsInputType>
			requires std::is_constructible_v<SettingsInputType, graphics_environment_settings>
		graphics_environment(SettingsInputType settings) : _settings(settings)
			{ setup(); }
		/* If not yet set up, sets up program-wide logic.
		 * Must be called from the thread the main function was called by.
		 * @throws std::logic_error when an instance of graphics_environment already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		graphics_environment() : graphics_environment(graphics_environment_settings()) {}
		/* Must be called from the main graphics thread, or else is undefined behaviour.
		 * If the environment is the last in the program, tears down program-wide logic.
		 */
		~graphics_environment();

		graphics_environment(const graphics_environment&) = delete;
		graphics_environment(graphics_environment&&) = delete;
		auto operator=(const graphics_environment&)->graphics_environment = delete;
		auto operator=(graphics_environment&&)->graphics_environment = delete;
	private:
		void setup();

	public:
		/* Handles events received from outside the program.
		 * @throws std::logic_error if not run by the main graphics thread.
		 */
		void update();
		/* Invoked right before the environment handles events received from outside the program.
		 * @see update()
		 */
		event<graphics_environment_update_parameter> updating;
		/* Invoked right after the environment handles events received from outside the program.
		 * @see update()
		 */
		event<graphics_environment_update_parameter> updated;

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

	public:
		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
		 * Returns the vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance get_vulkan_instance() const noexcept
		{
			return _vulkan_handle.get_vulkan_instance();
		}
	};

	extern template graphics_environment::graphics_environment(graphics_environment_settings);
	extern template graphics_environment::graphics_environment(const graphics_environment_settings&);
	extern template graphics_environment::graphics_environment(graphics_environment_settings&&);
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
