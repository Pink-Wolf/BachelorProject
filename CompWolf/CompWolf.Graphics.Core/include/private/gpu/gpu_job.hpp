#ifndef COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER

#include "gpu.hpp"
#include <stdexcept>
#include "utility"

namespace CompWolf::Graphics
{
	/* An identifier for a job on a gpu. */
	class gpu_job
	{
	private:
		/* The gpu containing the thread that the job is running on. */
		gpu* _device = nullptr;
		/* The family containing the thread that the job is running on. */
		size_t _family_index;
		/* The index of the thread in the family that the job is running on. */
		size_t _thread_index;

		bool _is_persistent;

	public:
		inline auto device() noexcept -> gpu&
		{
			return *_device;
		}
		inline auto family() noexcept -> gpu_thread_family&
		{
			return device().families()[_family_index];
		}
		inline auto thread() noexcept -> gpu_thread&
		{
			return family().threads[_thread_index];
		}
		
		inline auto device() const noexcept -> const gpu&
		{
			return *_device;
		}
		inline auto family() const noexcept -> const gpu_thread_family&
		{
			return device().families()[_family_index];
		}
		inline auto thread() const noexcept -> const gpu_thread&
		{
			return family().threads[_thread_index];
		}
		inline auto is_persistent() const noexcept -> bool
		{
			return _is_persistent;
		}

		inline auto empty() noexcept -> bool
		{
			return !_device;
		}

	public:
		gpu_job(gpu& device, size_t family_index, size_t thread_index, bool is_persistent);

		gpu_job() = default;
		gpu_job(gpu_job&&);
		auto operator=(gpu_job&&) -> gpu_job&;
		~gpu_job();

		gpu_job(const gpu_job&) = delete;
		auto operator=(const gpu_job&) -> gpu_job& = delete;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
