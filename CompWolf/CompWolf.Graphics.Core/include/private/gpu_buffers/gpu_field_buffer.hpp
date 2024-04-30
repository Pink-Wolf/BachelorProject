#ifndef COMPWOLF_GRAPHICS_GPU_FIELD_BUFFER_HEADER
#define COMPWOLF_GRAPHICS_GPU_FIELD_BUFFER_HEADER

#include "gpu_vkBuffer_allocator.hpp"
#include "gpu_buffer_access.hpp"
#include "shaders"
#include <utility>

namespace CompWolf
{
	/* Contains field-data for one or more shaders. If the data is an image, gpu_image_buffer should be used instead. */
	template <ShaderField ValueType>
	class gpu_field_buffer : public Private::base_gpu_vkBuffer_buffer
	{
	public: // type definitions
		/* The type of data in the buffer. */
		using value_type = ValueType;

	public: // accessors
		/* Returns a gpu_buffer_access to read and modify the buffer's data.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		inline auto data() { return gpu_buffer_access<value_type>(this); }
		/* Returns a gpu_buffer_access to read and modify the buffer's data.
		 * @throws std::invalid_argument if the buffer has more than 1 element.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		inline auto single_data() { return gpu_buffer_single_access<value_type>(this); }

	public: // constructors
		/* Constructs a freed gpu_field_buffer, as in one that has no data. */
		gpu_field_buffer() = default;
		gpu_field_buffer(gpu_field_buffer&&) = default;
		auto operator=(gpu_field_buffer&&) -> gpu_field_buffer& = default;
		inline ~gpu_field_buffer() noexcept { free(); }

		/* Constructs a gpu_field_buffer on the given gpu, with the given amount of elements.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_field_buffer(gpu_connection& gpu, std::size_t size)
			: base_gpu_vkBuffer_buffer(gpu, gpu_vkBuffer_type::uniform, size, sizeof(value_type))
		{}
		/* Constructs a gpu_field_buffer on the given gpu, with the given data.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_field_buffer(gpu_connection& gpu, std::initializer_list<value_type> data)
			: base_gpu_vkBuffer_buffer(gpu, gpu_vkBuffer_type::uniform, std::move(data))
		{}
		/* Constructs a gpu_field_buffer on the given gpu, with the given data.
		 * @throws std::runtime_error if there was an error during allocation due to causes outside of the program, including the gpu not having space.
		 */
		inline gpu_field_buffer(gpu_connection & gpu, value_type data)
			: base_gpu_vkBuffer_buffer(gpu, gpu_vkBuffer_type::uniform, { std::move(data) })
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FIELD_BUFFER_HEADER
