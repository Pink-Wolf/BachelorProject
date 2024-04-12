#include "pch.h"
#include "gpus"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/******************************** accessors ********************************/

	auto gpu_job::find_thread_for_job(const gpu_thread_family& family) noexcept -> std::size_t
	{
		std::size_t best_thread_index = 0;
		if (family.threads[best_thread_index].job_count == 0) return best_thread_index;

		for (size_t thread_index = 1; thread_index < family.threads.size(); ++thread_index)
		{
			auto job_count = family.threads[best_thread_index].job_count;
			if (job_count < family.threads[best_thread_index].job_count)
			{
				best_thread_index = thread_index;
				if (job_count == 0) return thread_index;
			}
		}
		return best_thread_index;
	}
	auto gpu_job::find_family_for_job(gpu_job_settings& settings, const gpu_connection& gpu, float& best_score, float& best_custom_score) noexcept
		-> std::optional<std::size_t>
	{
		const gpu_thread_family* best_family = nullptr;
		std::size_t best_family_index = 0;
		best_score = std::numeric_limits<float>::lowest();

		for (std::size_t family_index = 0; family_index < gpu.families().size(); ++family_index)
		{
			auto& family = gpu.families()[family_index];

			auto additional_work_types = family.job_types ^ settings.type;
			if ((additional_work_types & settings.type).any()) continue;

			std::optional<float> custom_score_container = settings.family_scorer ? settings.family_scorer(family) : std::optional<float>(0.f);
			if (!custom_score_container.has_value()) continue;
			auto custom_score = custom_score_container.value();

			float score = 0.f;
			{
				score -= additional_work_types.count() / static_cast<float>(gpu_job_type::size);

				score -= static_cast<float>(family.job_count / family.threads.size()); // Truncation of "job-count / thread-count" is intentional
			}

			constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_job_type::size);
			bool is_better = score > best_score + very_small_score_difference;
			if (best_score - score < very_small_score_difference) is_better = is_better || custom_score > best_custom_score; // use custom_score if scores are same

			if (is_better)
			{
				best_family = &family;
				best_family_index = family_index;
				best_score = score;
				best_custom_score = custom_score;
			}
		}

		if (!best_family) return std::nullopt;
		return best_family_index;
	}
	auto gpu_job::find_family_for_job(gpu_job_settings& settings, const gpu_manager& manager) noexcept
		-> std::optional<std::pair<size_t, std::size_t>>
	{
		gpu_connection* best_gpu = nullptr;
		std::size_t best_gpu_index = 0;
		std::size_t best_family_index = 0;
		float best_score = std::numeric_limits<float>::lowest();
		float best_custom_score;

		for (auto& gpu : manager.gpus())
		{
			auto additional_work_types_for_gpu = gpu.work_types() ^ settings.type;
			if ((additional_work_types_for_gpu & settings.type).any()) continue;

			std::optional<float> custom_gpu_score_container = settings.gpu_scorer ? settings.gpu_scorer(gpu) : std::optional<float>(0.f);
			if (!custom_gpu_score_container.has_value()) continue;
			auto custom_gpu_score = custom_gpu_score_container.value();

			float score, custom_score;
			auto index_container = find_family_for_job(settings, gpu, score, custom_score);
			custom_score += custom_gpu_score;

			if (!index_container.has_value()) continue;
			auto family_index = index_container.value();

			constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_job_type::size);
			bool is_better = score > best_score + very_small_score_difference;
			if (best_score - score < very_small_score_difference) is_better = is_better || custom_score > best_custom_score; // use custom_score if scores are same

			if (is_better)
			{
				best_family_index = family_index;
				best_score = score;
				best_custom_score = custom_score;
			}
		}

		return std::make_pair(best_gpu_index, best_family_index);
	}

	/******************************** constructors ********************************/

	gpu_job::gpu_job(gpu_connection& device_in, std::size_t family_index_in, std::size_t thread_index_in)
		: _device(&device_in)
		, _family_index(family_index_in)
		, _thread_index(thread_index_in)
		, _vulkan_pool(nullptr)
	{
		try
		{
			++family().job_count;
			++thread().job_count;

			auto logicDevice = Private::to_vulkan(device().vulkan_device());

			VkCommandPool commandPool;
			{
				VkCommandPoolCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
					.queueFamilyIndex = static_cast<uint32_t>(family_index()),
				};

				auto result = vkCreateCommandPool(logicDevice, &createInfo, nullptr, &commandPool);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"command pool\", used to store gpu-programs on the gpu.");
				}

				_vulkan_pool = Private::from_vulkan(commandPool);
			}
		}
		catch (...)
		{
			free();
			throw;
		}
	}

	auto gpu_job::new_job_for(gpu_connection& gpu, gpu_job_settings settings) -> gpu_job
	{
		float a, b;
		auto i = gpu_job::find_family_for_job(settings, gpu, a, b);
		if (!i) throw std::runtime_error("The machine's GPUs could not perform a job because of the type of work it requires.");

		auto family_index = i.value();
		auto thread_index = find_thread_for_job(gpu.families()[family_index]);
		return gpu_job(gpu, family_index, thread_index);
	}
	auto gpu_job::new_job_for(gpu_manager& manager, gpu_job_settings settings) -> gpu_job
	{
		auto i = gpu_job::find_family_for_job(settings, manager);
		if (!i) throw std::runtime_error("The machine's GPUs could not perform a job because of the type of work it requires.");

		auto [gpu_index, family_index] = i.value();
		auto& gpu = manager.gpus()[gpu_index];
		auto thread_index = find_thread_for_job(gpu.families()[family_index]);
		return gpu_job(gpu, family_index, thread_index);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_job::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		vkDeviceWaitIdle(logicDevice);
		vkDestroyCommandPool(logicDevice, Private::to_vulkan(_vulkan_pool), nullptr);

		--family().job_count;
		--thread().job_count;

		_device = nullptr;
	}
}
