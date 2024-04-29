#ifndef COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_ALLOCATOR_HEADER
#define COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_ALLOCATOR_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct gpu_buffer_private_info;
	}
	class gpu_connection;

	/* Base, abstract class used by base_gpu_buffer to actually allocate memory on the gpu, and get information about that memory. */
	class base_gpu_buffer_allocator : public basic_freeable
	{
	public: // type definitions
		/* Represents some data allocated by the allocator; the exact vulkan type this represents depends on the implementation. */
		using data_handle = Private::vulkan_handle64<struct data_handle_t>;

	private: // fields
		owned_ptr<gpu_connection*> _gpu;

	public: // accessors
		/* Returns the gpu that the allocator can allocate memory on. */
		inline auto& gpu() noexcept { return *_gpu; }
		/* Returns the gpu that the allocator can allocate memory on. */
		inline auto& gpu() const noexcept { return *_gpu; }

	public: // virtual
		/* Allocates some memory for the data to be send to and then read by the gpu.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program.
		 */
		virtual auto alloc_data() const -> data_handle = 0;
		/* Allocates some memory for the cpu to write to, before being send to the given data that the gpu can read.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program.
		 */
		virtual auto alloc_cpu_access(data_handle) const -> data_handle = 0;
		/* Binds the allocated data to the given vulkan_memory.
		 * @throws std::runtime_error if there was an error binding due to causes outside of the program.
		 */
		virtual void bind_data(data_handle, Private::vulkan_memory) const = 0;
		/* Deallocates the given data that was allocated with alloc_cpu_access. */
		virtual void free_cpu_access(data_handle) const noexcept = 0;
		/* Deallocates the given data that was allocated with alloc_data. */
		virtual void free_data(data_handle) const noexcept = 0;
		/* Gets information about the given allocated data.
		 * @throws std::runtime_error if there was an error getting the info due to causes outside of the program.
		 */
		virtual void private_info(data_handle data, Private::gpu_buffer_private_info* out) const = 0;

	public: // constructors
		/* Constructs a freed base_gpu_buffer_allocator, as in one that cannot be used to allocate memory. */
		base_gpu_buffer_allocator() = default;
		base_gpu_buffer_allocator(const base_gpu_buffer_allocator&) = default;
		auto operator=(const base_gpu_buffer_allocator&) -> base_gpu_buffer_allocator& = default;
		base_gpu_buffer_allocator(base_gpu_buffer_allocator&&) = default;
		auto operator=(base_gpu_buffer_allocator&&) -> base_gpu_buffer_allocator& = default;
		inline ~base_gpu_buffer_allocator() noexcept { free(); }

		/* Constructs a base_gpu_buffer_allocator that can allocate memory on the given gpu. */
		inline base_gpu_buffer_allocator(gpu_connection& gpu) noexcept : _gpu(&gpu) {}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final { return !_gpu; }
		inline void free() noexcept { _gpu = nullptr; }
	};
}

#endif // ! COMPWOLF_GRAPHICS_BASE_GPU_BUFFER_ALLOCATOR_HEADER
