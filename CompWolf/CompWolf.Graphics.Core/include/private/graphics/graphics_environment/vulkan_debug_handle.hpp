#ifndef COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER
#define COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"

namespace CompWolf::Graphics
{
	/* Contains debugging logic for vulkan, used by CompWolf::Graphics. */
	class vulkan_debug_handle
	{
	private:
		Private::vulkan_debug_messenger* _messenger;
		std::function<void(Private::vulkan_debug_messenger*)> _teardowner;
	public:
		/* Constructs an uninitialized vulkan_debug_handle. */
		vulkan_debug_handle() = default;
		/* Should only be constructed by graphics_environment.
		 * @param settings How the vulkan_debug_handle should behave. The object must stay alive throughout vulkan_debug_handle's lifetime.
		 * @param instance The instance of vulkan to debug.
		 * @throws std::logic_error when an instance of vulkan_debug_handle already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		vulkan_debug_handle(const graphics_environment_settings& settings, Private::vulkan_instance* instance);
		~vulkan_debug_handle();

		vulkan_debug_handle(vulkan_debug_handle&&) noexcept;
		vulkan_debug_handle& operator=(vulkan_debug_handle&&) noexcept;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER
