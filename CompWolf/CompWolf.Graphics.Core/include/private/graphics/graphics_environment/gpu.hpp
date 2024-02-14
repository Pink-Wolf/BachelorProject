#ifndef COMPWOLF_GRAPHICS_GPU_HEADER
#define COMPWOLF_GRAPHICS_GPU_HEADER

#include "vulkan_types"
#include <vector>

namespace CompWolf::Graphics
{
	/* Contains a connection to each gpu on the machine that can be used by CompWolf::Graphics. */
	class gpu
	{
	public:
		/* The connection to the gpu. */
		Private::vulkan_device _vulkan_device;

	public:
		/* Constructs a gpu that is not connected to any actual gpu. */
		gpu() = default;
		/* @param device The actual graphics processing unit to make a gpu-object for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		explicit gpu(Private::vulkan_physical_device device);
		~gpu();

		gpu(gpu&& other) noexcept;
		gpu& operator=(gpu&& other) noexcept;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_HEADER
