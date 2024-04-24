#ifndef COMPWOLF_GRAPHICS_GPU_INDEX_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_INDEX_BUFFER_HEADER

#include "gpu_vkBuffer_allocator.hpp"
#include "gpu_buffer_access.hpp"
#include "shaders"

namespace CompWolf::Graphics
{
	/* Contains index-data for one or more input_shaders. This specifies which of the shader's inputs, and in what order, it should use. */
	class gpu_index_buffer : public Private::base_gpu_vkBuffer_buffer
	{
	public: // type definitions
		/* The type of data in the buffer. */
		using value_type = shader_int;

	public: // accessors
		/* Returns a gpu_buffer_access to read and modify the buffer's data.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		inline auto data() { return gpu_buffer_access<value_type>(this); }

	public: // constructors
		/* Constructs a freed gpu_index_buffer, as in one that has no data. */
		gpu_index_buffer() = default;
		gpu_index_buffer(gpu_index_buffer&&) = default;
		auto operator=(gpu_index_buffer&&) -> gpu_index_buffer& = default;
		inline ~gpu_index_buffer() noexcept { free(); }

		/* Constructs a gpu_index_buffer on the given gpu, with the given amount of elements.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_index_buffer(gpu_connection& gpu, std::size_t size)
			: base_gpu_vkBuffer_buffer(gpu, gpu_vkBuffer_type::index, size, sizeof(value_type))
		{}
		/* Constructs a gpu_index_buffer on the given gpu, with the given data.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_index_buffer(gpu_connection& gpu, std::initializer_list<value_type> data)
			: base_gpu_vkBuffer_buffer(gpu, gpu_vkBuffer_type::index, std::move(data))
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_INDEX_BUFFER_HEADER
