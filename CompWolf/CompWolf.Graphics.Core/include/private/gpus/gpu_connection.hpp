#ifndef COMPWOLF_GRAPHICS_GPU_HEADER
#define COMPWOLF_GRAPHICS_GPU_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>
#include "gpu_thread_family.hpp"
#include <vector>

namespace CompWolf::Graphics
{
	class gpu_job;
	struct gpu_job_settings;

	/* A connection to a gpu. */
	class gpu_connection : public basic_freeable
	{
	private: // fields
		/* The gpu's vulkan_instance, representing a VkInstance. */
		owned_ptr<Private::vulkan_instance> _vulkan_instance;
		/* The gpu's vulkan_physical_device, representing a VkPhysicalDevice. */
		Private::vulkan_physical_device _vulkan_physical_device;
		/* The gpu's vulkan_device, representing a VkDevice. */
		Private::vulkan_device _vulkan_device;
		/* The gpu's families of threads. */
		std::vector<gpu_thread_family> _families;
		/* The types of work at least 1 thread can perform on the gpu. */
		gpu_job_type_set _work_types;

	public: // accessors
		/* Returns the gpu's families of threads. */
		inline auto& families() noexcept { return _families; }
		/* Returns the gpu's families of threads. */
		inline auto& families() const noexcept { return _families; }

		/* Returns the index of the given family in the vector returned by families().
		 * @throws std::invalid_argument if the gpu does not have the given thread.
		 */
		auto index_of_family(const gpu_thread_family&) const -> std::size_t;

		/* Returns the types of work at least 1 thread can perform on the gpu. */
		inline auto work_types() const noexcept { return _work_types; }

	public: // vulkan-related
		/* Returns the gpu's vulkan_instance, representing a VkInstance. */
		inline auto vulkan_instance() const noexcept { return _vulkan_instance; }
		/* Returns the gpu's vulkan_physical_device, representing a VkPhysicalDevice. */
		inline auto vulkan_physical_device() const noexcept { return _vulkan_physical_device; }
		/* Returns the gpu's vulkan_device, representing a VkDevice. */
		inline auto vulkan_device() const noexcept { return _vulkan_device; }

	public: // constructors
		/* Constructs a freed gpu_connection, that is a gpu_connection that does not represent an actual connection. */
		gpu_connection() = default;
		gpu_connection(gpu_connection&&) = default;
		gpu_connection& operator=(gpu_connection&&) = default;
		inline ~gpu_connection() noexcept { free(); }

		/* Sets up a connection to the given gpu.
		 * @throws std::runtime_error if there was an error while setting up the connection due to causes outside of the program.
		 */
		gpu_connection(Private::vulkan_instance vulkan_instance, Private::vulkan_physical_device device);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_vulkan_instance;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_HEADER
