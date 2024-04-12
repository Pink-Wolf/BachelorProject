#ifndef COMPWOLF_GRAPHICS_GPU_JOB_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>
#include "gpu_connection.hpp"
#include "gpu_job_settings.hpp"
#include "gpu_job_sync.hpp"
#include "utility"
#include <vector>
#include <optional>

namespace CompWolf::Graphics
{
	class gpu_manager;

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

		/* The job's vulkan_command_pool, representing a VkCommandPool. */
		Private::vulkan_command_pool _vulkan_pool;
		/* The job's synchronization-data. */
		std::vector<gpu_job_sync> _syncs;

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

		/* Returns a pointer to the job's latest synchronization-data.
		 * Returns nullptr if there are no synchronization data.
		 */
		auto latest_synchronization() const noexcept { return _syncs.empty() ? nullptr : &_syncs.back(); }
		/* Returns a pointer to the job's latest synchronization-data.
		 * Returns nullptr if there are no synchronization data.
		 */
		auto latest_synchronization() noexcept { return _syncs.empty() ? nullptr : &_syncs.back(); }

		/* Returns true if the job has unfinished work, otherwise returns false. */
		inline auto working() const noexcept
		{
			return latest_synchronization() ? !latest_synchronization()->fence.signaled() : false;
		}

	private:
		static auto find_thread_for_job(const gpu_thread_family&) noexcept -> std::size_t;
		static auto find_family_for_job(gpu_job_settings&, const gpu_connection&, float& out_score, float& out_custom_score) noexcept
			-> std::optional<std::size_t>;
		static auto find_family_for_job(gpu_job_settings&, const gpu_manager&) noexcept
			-> std::optional<std::pair<size_t, std::size_t>>;

	public: // modifiers
		/* Waits until all of the job's work is done, and then returns. */
		inline void wait() const noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
		}
		/* Waits until all of the job's work is done, and then returns. */
		inline void wait() noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
			_syncs.clear();
		}

		/* Replaces the job's latest synchronization-data; the new data should denote when all of the job's work is done. */
		inline auto& push_synchronization(gpu_job_sync&& s) noexcept
		{
			return _syncs.emplace_back(std::move(s));
		}

	public: // vulkan-related
		/* Returns the job's vulkan_command_pool, representing a VkCommandPool. */
		inline auto vulkan_pool() const noexcept { return _vulkan_pool; }

		/* Returns the latest synchronization-data's vulkan_fence, representing a VkFence.
		 * Returns nullptr if there are no synchronization data.
		 */
		inline auto last_vulkan_fence() const noexcept -> Private::vulkan_fence
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().fence.vulkan_fence();
		}

		/* Returns the latest synchronization-data's vulkan_semaphore, representing a VkSemaphore.
		 * Returns nullptr if there are no synchronization data.
		 */
		inline auto last_vulkan_semaphore() const noexcept -> Private::vulkan_semaphore
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().semaphore.vulkan_semaphore();
		}

	public: // constructors
		/* Constructs a freed gpu_job, as in one that does not reference any thread. */
		gpu_job() = default;
		gpu_job(gpu_job&&) = default;
		auto operator=(gpu_job&&) -> gpu_job& = default;

		/* Creates a job referencing a thread on the given gpu, whose family is at the given family-index of the gpu's families()-vector, and who is at the given thread-index of the family's threads-vector.
		 * @throws std::out_of_range if the given gpu does not have a family at the given family-index, or the family does not have a thread at the given thread-index.
		 */
		gpu_job(gpu_connection& device, std::size_t family_index, std::size_t thread_index);

		/* Finds the best thread on the given gpu to perform some work, based on the given settings, and creates a job on it using gpu_job(gpu_connection&, std::size_t, std::size_t).
		 * @throws std::runtime_error from (3) and (4) if the given gpus have no threads at all to perform the given type of work.
		 */
		static auto new_job_for(gpu_connection&, gpu_job_settings) -> gpu_job;
		/* Finds the best thread on the given gpus to perform some work, based on the given settings, and creates a job on it using gpu_job(gpu_connection&, std::size_t, std::size_t).
		 * @throws std::runtime_error from (3) and (4) if the given gpus have no threads at all to perform the given type of work.
		 */
		static auto new_job_for(gpu_manager&, gpu_job_settings) -> gpu_job;

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_HEADER
