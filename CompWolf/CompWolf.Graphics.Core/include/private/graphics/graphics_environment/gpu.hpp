#ifndef COMPWOLF_GRAPHICS_GPU_HEADER
#define COMPWOLF_GRAPHICS_GPU_HEADER

#include "vulkan_types"
#include <vector>
#include <enum_bitset>
#include <map>

namespace CompWolf::Graphics
{
	/* The different type of work a gpu can do. */
	enum class gpu_work_type : std::size_t
	{
		/* Drawing on a window or image. */
		draw,
		/* Make a window actually show what has been drawn on it. */
		present,
		size
	};

	/* A collection of thread which can do the same type of work.
	 * On a lower abstraction layer, a queue family.
	 */
	struct gpu_thread_family
	{
		/* The type of work the threads can do. */
		enum_bitset<gpu_work_type> work_types;
		/* The amount of threads in the family. */
		size_t size;
	};

	/* Contains a connection to a gpu on the machine that can be used by CompWolf::Graphics. */
	class gpu
	{
	private:
		/* The connection to the gpu. */
		Private::vulkan_device _vulkan_device;

		std::vector<gpu_thread_family> _families;

	public:
		/* Constructs a gpu that is not connected to any actual gpu. */
		gpu() = default;
		/* Should only be constructed by graphics_environment.
		 * @param device The actual graphics processing unit to make a gpu-object for.
		 * @throws std::runtime_error when something went wrong during setup outside of the program.
		 */
		explicit gpu(Private::vulkan_instance vulkan_instance, Private::vulkan_physical_device device);
		~gpu();

		gpu(gpu&& other) noexcept;
		gpu& operator=(gpu&& other) noexcept;

	public:
		/* Gets all of the gpu's threads, split into various "families".
		 * On a lower abstraction layer, gets all of the gpu's queues, split into their different queue families.
		 */
		inline const std::vector<gpu_thread_family>& families() const
		{
			return _families;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_HEADER
