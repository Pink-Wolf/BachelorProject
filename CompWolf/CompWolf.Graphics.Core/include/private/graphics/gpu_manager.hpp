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
		gpu_manager() noexcept = default;
		/* @throws std::runtime_error when something went wrong during setup outside of the program. */
		explicit gpu_manager(Private::vulkan_instance* instance);
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
