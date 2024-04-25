#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>
#include "gpus"
#include "gpu_program_manager_settings.hpp"
#include "gpu_program_sync.hpp"
#include "utility"
#include <vector>
#include <optional>

namespace CompWolf::Graphics
{
	/* A manager for some gpu_programs.
	 * Programs should be split into different managers when they can run asynchronous.
	 */
	class gpu_program_manager : public basic_freeable
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
		std::vector<gpu_program_sync> _syncs;

	public: // accessors
		/* Returns the gpu that the manager is on. */
		inline auto& device() noexcept { return *_device; }
		/* Returns the gpu that the manager is on. */
		inline auto& device() const noexcept { return *_device; }

		/* Returns the family of threads that the manager is on. */
		inline auto& family() noexcept { return device().families()[_family_index]; }
		/* Returns the family of threads that the manager is on. */
		inline auto& family() const noexcept { return device().families()[_family_index]; }

		/* Returns the family of threads that the manager is on. */
		inline auto& thread() noexcept { return family().threads[_thread_index]; }
		/* Returns the family of threads that the manager is on. */
		inline auto& thread() const noexcept { return family().threads[_thread_index]; }

		/* Returns the index of the gpu-thread-family in the gpu's families()-vector. */
		inline auto family_index() const noexcept { return _family_index; }
		/* Returns the index of the gpu-thread in the gpu-thread-family's threads-vector. */
		inline auto thread_index() const noexcept { return _thread_index; }

		/* Returns a pointer to the manager's latest synchronization-data.
		 * Waiting on the data's fence waits for all of the manager's programs to finish.
		 * Returns nullptr if there are no synchronization data.
		 */
		auto latest_synchronization() const noexcept { return _syncs.empty() ? nullptr : &_syncs.back(); }
		/* Returns a pointer to the manager's latest synchronization-data.
		 * Waiting on the data's fence waits for all of the manager's programs to finish.
		 * Returns nullptr if there are no synchronization data.
		 */
		auto latest_synchronization() noexcept { return _syncs.empty() ? nullptr : &_syncs.back(); }

		/* Returns whether any of the programs are still running. */
		inline auto working() const noexcept
		{
			return latest_synchronization() ? !latest_synchronization()->fence.signaled() : false;
		}

	private:
		static auto find_thread(const gpu_thread_family&) noexcept -> std::size_t;
		static auto find_family(gpu_program_manager_settings&, const gpu_connection&, float& out_score, float& out_custom_score) noexcept
			-> std::optional<std::size_t>;
		static auto find_family(gpu_program_manager_settings&, const std::vector<gpu_connection>&) noexcept
			-> std::optional<std::pair<size_t, std::size_t>>;

	public: // modifiers
		/* Waits until all of the programs are done, and then returns. */
		inline void wait() const noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
		}
		/* Waits until all of the programs are done, and then returns. */
		inline void wait() noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
			_syncs.clear();
		}

		/* Replaces the manager's latest synchronization-data; the new data should denote when all of the manager's programs are done. */
		inline auto& push_synchronization(gpu_program_sync&& s) noexcept
		{
			return _syncs.emplace_back(std::move(s));
		}

	public: // vulkan-related
		/* Returns the manager's vulkan_command_pool, representing a VkCommandPool. */
		inline auto vulkan_pool() const noexcept { return _vulkan_pool; }

		/* Returns the latest synchronization-object's vulkan_fence, representing a VkFence.
		 * Returns nullptr if there are no synchronization objects.
		 */
		inline auto last_vulkan_fence() const noexcept -> Private::vulkan_fence
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().fence.vulkan_fence();
		}

		/* Returns the latest synchronization-object's vulkan_semaphore, representing a VkSemaphore.
		 * Returns nullptr if there are no synchronization objects.
		 */
		inline auto last_vulkan_semaphore() const noexcept -> Private::vulkan_semaphore
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().semaphore.vulkan_semaphore();
		}

	public: // constructors
		/* Constructs a freed gpu_program_manager, as in one that cannot contain any programs. */
		gpu_program_manager() = default;
		gpu_program_manager(gpu_program_manager&&) = default;
		auto operator=(gpu_program_manager&&) -> gpu_program_manager& = default;

		/* Creates a manager on the given thread;
		 * The given thread is the thread on the given gpu, whose family is at the given family-index of the gpu's families()-vector, and who is at the given thread-index of the family's threads-vector.
		 * @throws std::out_of_range if the given gpu does not have a family at the given family-index, or the family does not have a thread at the given thread-index.
		 */
		gpu_program_manager(gpu_connection& device, std::size_t family_index, std::size_t thread_index);

		/* Finds the best thread on the given gpu to run some programs, based on the given settings, and creates a manager on it.
		 * @throws std::runtime_error if the given gpus have no threads at all to perform the given type of programs.
		 */
		static auto new_manager_for(gpu_connection&, gpu_program_manager_settings) -> gpu_program_manager;
		/* Finds the best thread on the given gpus to run some programs, based on the given settings, and creates a manager on it.
		 * @throws std::runtime_error if the given gpus have no threads at all to perform the given type of programs.
		 */
		static auto new_manager_for(std::vector<gpu_connection>&, gpu_program_manager_settings) -> gpu_program_manager;

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_POOL_HEADER
