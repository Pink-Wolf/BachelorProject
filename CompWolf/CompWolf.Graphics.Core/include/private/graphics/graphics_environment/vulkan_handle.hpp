#ifndef COMPWOLF_GRAPHICS_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_VULKAN_HEADER

#include "vulkan_types"
#include "graphics_environment_settings.hpp"

namespace CompWolf::Graphics
{
	/* Contains program-wide logic for vulkan, used by CompWolf::Graphics. */
	class vulkan_handle
	{
	private:
		Private::vulkan_instance* _vulkan_instance;
	public:
		/* Constructs a vulkan_handle without setting up any logic. */
		vulkan_handle() = default;
		/* Should only be constructed by graphics_environment.
		 * @param settings How the vulkan_handle should behave. The object must stay alive throughout vulkan_handle's lifetime.
		 * @throws std::logic_error when an instance of vulkan_handle already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		vulkan_handle(const graphics_environment_settings& settings);
		~vulkan_handle();

		vulkan_handle(vulkan_handle&&) noexcept;
		vulkan_handle& operator=(vulkan_handle&&) noexcept;

	public:
		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
		 * Returns the vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance* get_vulkan_instance() const noexcept
		{
			return _vulkan_instance;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_HEADER
