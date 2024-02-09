#ifndef COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER

#include "private/vulkan_types.h"
#include <thread>
#include <vector>
#include <event.h>
#include <empty_pointer.h>
#include <memory>

namespace CompWolf::Graphics
{
	struct graphics_environment_update_parameter
	{

	};

	/* Manages program-wide logic used to handle graphics and windows.
	 * Only a single instance of graphics_environment may exist at once in a program.
	 */
	class graphics_environment
	{
	private:
		struct teardown_glfw
		{
			using pointer = empty_pointer;
			void operator()(empty_pointer) const;
		};
		struct teardown_vulkan
		{
			using pointer = Private::vulkan_instance*;
			void operator()(Private::vulkan_instance*) const;
		};

		using glfw_handle_type = std::unique_ptr<empty_pointer_element, teardown_glfw>;
		using vulkan_handle_type = std::unique_ptr<Private::vulkan_instance, teardown_vulkan>;
	private:
		/* Whether an instance of the environment has already been constructed, which has not yet been destructed. */
		static bool constructed;

		/* The main thread for handling graphics and windows.
		 * This is the thread the main function was called by.
		 */
		std::thread::id main_graphics_thread;

		// The order of the following data members is important for their destructors to be called in the right order.
		/* Containing program-wide logic for glfw. */
		glfw_handle_type glfw_handle;
		/* Containing program-wide logic for vulkan. */
		vulkan_handle_type vulkan_handle;

	public:
		/* If not yet set up, sets up program-wide logic.
		 * Must be called from the thread the main function was called by.
		 * @throws std::logic_error when an instance of graphics_environment already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		graphics_environment();
		/* Must be called from the main graphics thread, or else is undefined behaviour.
		 * If the environment is the last in the program, tears down program-wide logic.
		 */
		~graphics_environment();
	private:
		static auto setup_glfw() -> glfw_handle_type;
		static auto setup_vulkan() -> vulkan_handle_type;

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

	public:
		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
		 * Returns the environment's vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance* get_vulkan_instance() const noexcept { return vulkan_handle.get(); }
	};
}

#endif // ! COMPWOLF_GRAPHICS_GRAPHICS_ENVIRONMENT_HEADER
