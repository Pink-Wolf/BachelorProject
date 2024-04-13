#ifndef COMPWOLF_GRAPHICS_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_VULKAN_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	/* Contains program-wide logic for vulkan, used by CompWolf::Graphics. */
	class vulkan_handle : public basic_freeable
	{
	private: // fields
		owned_ptr<Private::vulkan_instance> _vulkan_instance;

	public: // accessors
		/* Returns the handle's vulkan_instance, representing a VkInstance. */
		inline auto vulkan_instance() const noexcept { return _vulkan_instance; }

	public: // constructors
		/* Constructs an empty vulkan_handle, as in one that does not set up any logic. */
		vulkan_handle() = default;
		vulkan_handle(vulkan_handle&&) = default;
		auto operator=(vulkan_handle&&) -> vulkan_handle& = default;
		inline ~vulkan_handle() noexcept
		{
			free();
		}

		/* vulkan_handle should be constructed by graphics_environment.
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::logic_error when an instance of vulkan_handle already exists.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		vulkan_handle(const graphics_environment_settings& settings);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_instance;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_HEADER
