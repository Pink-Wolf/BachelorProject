#ifndef COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER

#include <freeable>
#include <owned>
#include "utility"

namespace CompWolf::Graphics
{
	class gpu_connection;

	/* A reference to a gpu-thread that is ready to perform some work. */
	class gpu_job : public basic_freeable
	{
	private: // fields
		/* The gpu that the job is on. */
		owned_ptr<gpu_connection*> _device;
		/* The index of the gpu-thread-family in the gpu's families()-vector. */
		std::size_t _family_index;
		/* The index of the gpu-thread in the gpu-thread-family's threads-vector. */
		std::size_t _thread_index;

	public: // accessors
		/* Returns the gpu that the job is on. */
		inline auto& device() noexcept { return *_device; }
		/* Returns the gpu that the job is on. */
		inline auto& device() const noexcept { return *_device; }

		/* Returns the family of threads that the job is on. */
		inline auto& family() noexcept { return device().families()[_family_index]; }
		/* Returns the family of threads that the job is on. */
		inline auto& family() const noexcept { return device().families()[_family_index]; }

		/* Returns the family of threads that the job is on. */
		inline auto& thread() noexcept { return family().threads[_thread_index]; }
		/* Returns the family of threads that the job is on. */
		inline auto& thread() const noexcept { return family().threads[_thread_index]; }

		/* Returns the index of the gpu-thread-family in the gpu's families()-vector. */
		inline auto family_index() const noexcept { return _family_index; }
		/* Returns the index of the gpu-thread in the gpu-thread-family's threads-vector. */
		inline auto thread_index() const noexcept { return _thread_index; }

	public: // constructors
		/* Constructs a freed gpu_job, as in one that does not reference any thread. */
		gpu_job() = default;
		gpu_job(gpu_job&&) = default;
		auto operator=(gpu_job&&) -> gpu_job& = default;

		/* Creates a job referencing a thread on the given gpu, whose family is at the given family-index of the gpu's families()-vector, and who is at the given thread-index of the family's threads-vector.
		 * @throws std::out_of_range if the given gpu does not have a family at the given family-index, or the family does not have a thread at the given thread-index.
		 */
		gpu_job(gpu_connection& device, std::size_t family_index, std::size_t thread_index);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
