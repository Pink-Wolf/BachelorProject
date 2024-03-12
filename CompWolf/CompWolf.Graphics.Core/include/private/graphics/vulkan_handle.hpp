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

	public: // getters
		/* This should rarely be used directly, as it exposes data of an abstaction layer lower than CompWolf::Graphics.
		 * Returns the vulkan instance, which handles vulkan-specific logic.
		 */
		inline Private::vulkan_instance vulkan_instance() const noexcept
		{
			return _vulkan_instance;
		}

	public: // constructors
		/* Constructs a vulkan_handle without setting up any logic. */
		vulkan_handle() = default;
		vulkan_handle(vulkan_handle&&) = default;
		auto operator=(vulkan_handle&&) -> vulkan_handle& = default;
		inline ~vulkan_handle() noexcept
		{
			free();
		}

		/* Should only be constructed by graphics_environment.
		 * @param settings How the vulkan_handle should behave. The object must stay alive throughout vulkan_handle's lifetime.
		 * @throws std::logic_error when an instance of vulkan_handle already exists.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
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
