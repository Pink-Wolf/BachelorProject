#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER

#include "vulkan_types"
#include <thread>
#include <type_traits>
#include <event>

#include "graphics_environment_settings.hpp"
#include "glfw_handle.hpp"
#include "vulkan_handle.hpp"
#include "vulkan_debug_handle.hpp"
#include "gpu"


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
	private: // fields
		/* Whether an instance of the environment has already been constructed, which has not yet been destructed. */
		static bool _constructed;
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

	public: // getters

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

		/* Returns the manager for the connections to the machine's gpus. */
		inline auto gpus() noexcept -> gpu_manager&
		{
			return _gpus;
		}
		/* Returns the manager for the connections to the machine's gpus. */
		inline auto gpus() const noexcept -> const gpu_manager&
		{
			return _gpus;
		}

		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
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
		/* Invoked right before the environment handles events received from outside the program.
		 * @see update()
		 */
		event<graphics_environment_update_parameter> updating;
		/* Invoked right after the environment handles events received from outside the program.
		 * @see update()
		 */
		event<graphics_environment_update_parameter> updated;

	private: // constructor
		void setup();
	public:
		graphics_environment(const graphics_environment&) = delete;
		auto operator=(const graphics_environment&) -> graphics_environment& = delete;
		graphics_environment(graphics_environment&&) = delete;
		auto operator=(graphics_environment&&) -> graphics_environment& = delete;

		/* If not yet set up, sets up program-wide logic.
		 * If not run on the thread running the main function, then this will have undefined behaviour.
		 * @param settings A graphics_environment_settings specifying how the environment should behave.
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

	};

	extern template graphics_environment::graphics_environment(graphics_environment_settings);
	extern template graphics_environment::graphics_environment(const graphics_environment_settings&);
	extern template graphics_environment::graphics_environment(graphics_environment_settings&&);
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
