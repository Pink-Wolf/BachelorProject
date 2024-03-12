#ifndef COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER

#include "gpu.hpp"
#include <stdexcept>
#include "utility"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	/* An identifier for a job on a gpu. */
	class gpu_job : public basic_freeable
	{
	private: // fields
		/* The gpu containing the thread that the job is running on. */
		owned_ptr<gpu*> _device;
		/* The family containing the thread that the job is running on. */
		size_t _family_index;
		/* The index of the thread in the family that the job is running on. */
		size_t _thread_index;

		bool _is_persistent;

	public: // getters
		inline auto device() noexcept -> gpu&
		{
			return *_device;
		}
		inline auto device() const noexcept -> const gpu&
		{
			return *_device;
		}

		inline auto family() noexcept -> gpu_thread_family&
		{
			return device().families()[_family_index];
		}
		inline auto family() const noexcept -> const gpu_thread_family&
		{
			return device().families()[_family_index];
		}

		inline auto thread() noexcept -> gpu_thread&
		{
			return family().threads[_thread_index];
		}
		
		inline auto thread() const noexcept -> const gpu_thread&
		{
			return family().threads[_thread_index];
		}

		inline auto is_persistent() const noexcept -> bool
		{
			return _is_persistent;
		}

		inline auto family_index() const noexcept -> size_t
		{
			return _family_index;
		}
		inline auto thread_index() const noexcept -> size_t
		{
			return _thread_index;
		}

	public: // constructors
		gpu_job() = default;
		gpu_job(gpu_job&&) = default;
		auto operator=(gpu_job&&)->gpu_job& = default;

		gpu_job(gpu& device, size_t family_index, size_t thread_index, bool is_persistent);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
