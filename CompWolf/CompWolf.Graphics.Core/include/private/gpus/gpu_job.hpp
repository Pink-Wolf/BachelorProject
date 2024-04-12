#ifndef COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
#define COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER

#include <freeable>
#include <owned>
#include "gpu_fence.hpp"
#include "gpu_semaphore.hpp"
#include "gpu_connection.hpp"
#include "gpu_job_settings.hpp"
#include "utility"
#include <vector>
#include <optional>

namespace CompWolf::Graphics
{
	class gpu_manager;

	/* A reference to a gpu-thread that is ready to perform some work. */
	class gpu_job : public basic_freeable
	{
	public: // types
		using synchronizations_element_type = std::pair<gpu_fence, gpu_semaphore>;
		using synchronizations_type = std::vector<synchronizations_element_type>;
	private: // fields
		/* The gpu that the job is on. */
		owned_ptr<gpu_connection*> _device;
		/* The index of the gpu-thread-family in the gpu's families()-vector. */
		std::size_t _family_index;
		/* The index of the gpu-thread in the gpu-thread-family's threads-vector. */
		std::size_t _thread_index;

		Private::vulkan_command_pool _vulkan_pool;
		synchronizations_type _synchronizations;

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

		inline auto synchronizations() noexcept -> synchronizations_type& { return _synchronizations; }
		inline auto synchronizations() const noexcept -> const synchronizations_type& { return _synchronizations; }

	private:
		static auto find_thread_for_job(const gpu_thread_family&) noexcept -> std::size_t;
		static auto find_family_for_job(gpu_job_settings&, const gpu_connection&, float& out_score, float& out_custom_score) noexcept
			-> std::optional<std::size_t>;
		static auto find_family_for_job(gpu_job_settings&, const gpu_manager&) noexcept
			-> std::optional<std::pair<size_t, std::size_t>>;

	public: // modifiers
		inline auto& insert_synchronization(gpu_fence&& fence, gpu_semaphore&& semaphore) noexcept
		{
			return synchronizations().emplace_back(std::move(fence), std::move(semaphore));
		}

	public: // vulkan-related
		inline auto vulkan_pool() const noexcept { return _vulkan_pool; }

		inline auto last_vulkan_fence() const noexcept -> Private::vulkan_fence
		{
			if (synchronizations().empty()) return nullptr;
			return synchronizations().back().first.vulkan_fence();
		}

		inline auto last_vulkan_semaphore() const noexcept -> Private::vulkan_semaphore
		{
			if (synchronizations().empty()) return nullptr;
			return synchronizations().back().second.vulkan_semaphore();
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

		static auto new_job_for(gpu_connection&, gpu_job_settings) -> gpu_job;
		static auto new_job_for(gpu_manager&, gpu_job_settings) -> gpu_job;

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_JOB_KEY_HEADER
