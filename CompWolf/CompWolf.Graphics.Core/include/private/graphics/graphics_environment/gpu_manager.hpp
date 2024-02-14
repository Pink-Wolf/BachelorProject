#ifndef COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
#define COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER

#include "gpu.hpp"
#include "vulkan_types"
#include <vector>

namespace CompWolf::Graphics
{
	/* Contains a connection to each gpu on the machine that can be used by CompWolf::Graphics. */
	class gpu_manager
	{
	private:
		using _gpus_type = std::vector<gpu>;
		/* The gpus contained by the manager. */
		_gpus_type _gpus;
	public:
		/* Constructs a manager with no gpus. */
		gpu_manager() noexcept = default;
		/* Should only be constructed by graphics_environment.
		 * @param settings How the gpu_manager should behave. The object must stay alive throughout gpu_manager's lifetime.
		 * @param instance The instance of vulkan to create connections to gpus for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program. */
		explicit gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance* instance);
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
