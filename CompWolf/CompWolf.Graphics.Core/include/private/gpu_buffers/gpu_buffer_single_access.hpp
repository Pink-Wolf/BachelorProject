#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_SINGLE_ACCESS_HEADER
#define COMPWOLF_GRAPHICS_GPU_BUFFER_SINGLE_ACCESS_HEADER

#include "base_gpu_buffer.hpp"
#include <type_traits>
#include <stdexcept>

namespace CompWolf
{
	/* A smart-pointer to a base_gpu_buffer's memory. That is, one that on construction gets cpu-access to the buffer's data, and on destruction/freeing releases the access.
	 * The buffer this accesses must only contain 1 element.
	 */
	template <typename ValueType>
	class gpu_buffer_single_access : private Private::base_gpu_buffer_access
	{
	public: // type definitions
		/* The type of data in the buffer. */
		using value_type = ValueType;

	public: // accessors
		/* Returns the buffer whose data this accesses. */
		inline auto& target_buffer() noexcept { return Private::base_gpu_buffer_access::target_buffer(); }
		/* Returns the buffer whose data this accesses. */
		inline auto& target_buffer() const noexcept { return Private::base_gpu_buffer_access::target_buffer(); }

	public: // operators
		auto operator->() noexcept { return static_cast<value_type*>(Private::base_gpu_buffer_access::target_buffer_data()); }
		auto operator->() const noexcept { return static_cast<const value_type*>(Private::base_gpu_buffer_access::target_buffer_data()); }

		auto& operator*() noexcept { return *this->operator->(); }
		auto& operator*() const noexcept { return *this->operator->(); }

	public: // constructor
		/* Constructs a freed gpu_buffer_access, as in one that has already released its access to any buffer. */
		gpu_buffer_single_access() = default;
		gpu_buffer_single_access(gpu_buffer_single_access&&) = default;
		auto operator=(gpu_buffer_single_access&&) -> gpu_buffer_single_access& = default;

		/* Constructs a gpu_buffer_access that gives access to the given buffer's data.
		 * @throws std::invalid_argument if the given buffer is a nullptr.
		 * @throws std::invalid_argument if the given buffer has more than 1 element.
		 * @throws std::runtime_error if there was an error getting cpu-access to the data due to causes outside of the program.
		 */
		inline explicit gpu_buffer_single_access(base_gpu_buffer* target)
			: Private::base_gpu_buffer_access(target)
		{
			if (target->size() > 1)
				throw std::invalid_argument("Tried creating a gpu_buffer_single_access for a buffer that did not have exactly 1 element.");
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_SINGLE_ACCESS_HEADER
