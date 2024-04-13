#ifndef COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER
#define COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"
#include <freeable>

namespace CompWolf::Graphics
{
	/* Contains program-wide logic for glfw, used by CompWolf::Graphics. */
	class glfw_handle : public basic_freeable
	{
	private: // fields
		bool _not_empty = false;

	public: // constructors
		/* Constructs an empty glfw_handle, as in one that does not set up any logic. */
		glfw_handle() = default;
		glfw_handle(glfw_handle&&) noexcept;
		glfw_handle& operator=(glfw_handle&&) noexcept;
		inline ~glfw_handle() noexcept
		{
			free();
		}

		/* glfw_handle should be constructed by graphics_environment.
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::logic_error when an instance of glfw_handle already exists.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		glfw_handle(const graphics_environment_settings& settings);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_not_empty;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GLFW_HANDLE_HEADER
