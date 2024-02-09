#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER

#include "vulkan_types.h"
#include <thread>
#include <vector>
#include <event.h>

namespace CompWolf::Graphics
{
	struct graphics_environment_update_parameter
	{

	};

	/* Manages program-wide logic used to handle graphics and windows. */
	class graphics_environment
	{
	private:
		/* The main thread for handling graphics and windows.
		 * This is the thread the main function was called by.
		 */
		static std::thread::id main_graphics_thread;

		/* How many graphics_environment-objects that exists. */
		static size_t environment_counter;

	public:
		/* If not yet set up, sets up program-wide logic.
		 * Must be called from the thread the main function was called by.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		graphics_environment();
		/* Must be called from the main graphics thread, or else is undefined behaviour.
		 * If the environment is the last in the program, tears down program-wide logic.
		 */
		~graphics_environment();

	public:
		/* Handles events received from outside the program. */
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
			return main_graphics_thread == id;
		}
		/* Returns whether the thread calling this is the main graphics thread. */
		inline auto is_this_main_thread() noexcept -> bool
		{
			return is_main_thread(std::this_thread::get_id());
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
