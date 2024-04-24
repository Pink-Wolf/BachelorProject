#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_ACCESS_HEADER
#define COMPWOLF_GRAPHICS_GPU_BUFFER_ACCESS_HEADER

#include "base_gpu_buffer.hpp"
#include <span>

namespace CompWolf::Graphics
{
	/* A smart-pointer to a base_gpu_buffer's memory.
	 * That is, one that on construction gets cpu-access to the buffer's data, and on destruction/freeing releases the access.
	 * Inherits from std::span to give convenient access to the data.
	 */
	template <typename ValueType>
	class gpu_buffer_access : private Private::base_gpu_buffer_access, public std::span<ValueType, std::dynamic_extent>
	{
	private: // fields
		using super_span = std::span<ValueType, std::dynamic_extent>;

	public: // accessors
		/* Returns the buffer whose data this accesses. */
		inline auto& target_buffer() noexcept { return Private::base_gpu_buffer_access::target_buffer(); }
		/* Returns the buffer whose data this accesses. */
		inline auto& target_buffer() const noexcept { return Private::base_gpu_buffer_access::target_buffer(); }

	public: // constructor
		/* Constructs a freed gpu_buffer_access, as in one that has already released its access to any buffer. */
		gpu_buffer_access() = default;
		gpu_buffer_access(gpu_buffer_access&&) = default;
		auto operator=(gpu_buffer_access&&) -> gpu_buffer_access& = default;

		/* Constructs a gpu_buffer_access that gives access to the given buffer's data.
		 * @throws std::invalid_argument if the given buffer is a nullptr.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		explicit gpu_buffer_access(base_gpu_buffer* target)
			: Private::base_gpu_buffer_access(target)
			, super_span(static_cast<ValueType*>(Private::base_gpu_buffer_access::target_buffer_data()), target->size())
		{}

	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_ACCESS_HEADER
