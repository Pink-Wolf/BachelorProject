#ifndef COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER
#define COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"

namespace CompWolf::Graphics
{
	/* Contains program-wide logic for glfw, used by CompWolf::Graphics. */
	class glfw_handle
	{
	public:
		/* Constructs a glfw_handle without setting up any logic. */
		glfw_handle() = default;
		/* Should only be constructed by graphics_environment.
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @param settings How the glfw_handle should behave. The object must stay alive throughout glfw_handle's lifetime.
		 * @throws std::logic_error when an instance of glfw_handle already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		glfw_handle(const graphics_environment_settings& settings);
		~glfw_handle();

		glfw_handle(const glfw_handle&) noexcept;
		glfw_handle& operator=(const glfw_handle&) noexcept;
		glfw_handle(glfw_handle&&) noexcept;
		glfw_handle& operator=(glfw_handle&&) noexcept;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER
