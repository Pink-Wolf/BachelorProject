#ifndef COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER
#define COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	/* Contains debugging logic for vulkan, used by CompWolf::Graphics. */
	class vulkan_debug_handle : public basic_freeable
	{
	private: // fields
		owned_ptr<Private::vulkan_debug_messenger> _messenger;
		std::function<void(Private::vulkan_debug_messenger)> _teardowner;

	public: // accessors
		/* Returns the handle's vulkan_debug_messenger, representing a VkDebugUtilsMessengerEXT. */
		inline auto vulkan_debug_messenger() const noexcept { return _messenger; }

	public: // constructors
		/* Constructs an empty vulkan_handle, as in one that does not set up any debugging logic. */
		vulkan_debug_handle() = default;
		vulkan_debug_handle(vulkan_debug_handle&&) = default;
		auto operator=(vulkan_debug_handle&&) -> vulkan_debug_handle& = default;
		inline ~vulkan_debug_handle() noexcept
		{
			free();
		}

		/* vulkan_debug_handle should be constructed by graphics_environment.
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::logic_error when an instance of vulkan_debug_handle already exists.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		vulkan_debug_handle(const graphics_environment_settings& settings, Private::vulkan_instance instance);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_messenger;
		}
		inline void free() noexcept final
		{
			if (!empty()) _teardowner(_messenger);
			_messenger = nullptr;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_DEBUG_HANDLE_HEADER
