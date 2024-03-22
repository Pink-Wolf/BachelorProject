#include "pch.h"
#include "gpu"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>
#include <map>
#include <algorithm>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** other methods ********************************/

	auto gpu_manager::new_persistent_job(gpu_job_settings settings) -> gpu_job
	{
		auto [gpu_pointer, family_index] = find_job_family(settings, true);
		auto& gpu_device = *gpu_pointer; auto& family = gpu_device.families()[family_index];

		auto thread_index = find_job_thread_in_family(settings, true, family);
		auto& thread = family.threads[thread_index];

		return gpu_job(gpu_device, family_index, thread_index, true);
	}

	auto gpu_manager::find_job_family(const gpu_job_settings& settings, bool is_persistent_job) -> std::pair<gpu*, std::size_t>
	{
		static auto is_occupied_persistent = [](gpu_thread a) { return a.persistent_job_count > 0; };
		static auto is_occupied_nonpersistent = [](gpu_thread a) { return a.job_count > 0; };
		auto is_occupied = is_persistent_job ? is_occupied_persistent : is_occupied_nonpersistent;

		gpu* best_gpu = nullptr;
		std::size_t best_family_index = 0;
		float best_family_score = std::numeric_limits<float>::lowest();
		float best_family_score_custom = 0;
		for (auto& gpu_item : _gpus)
		{
			auto additional_work_types_for_gpu = gpu_item.work_types() ^ settings.type;
			if ((additional_work_types_for_gpu & settings.type).any()) continue;

			std::optional<float> custom_gpu_score_container = settings.gpu_scorer ? settings.gpu_scorer(gpu_item) : std::optional<float>(0.f);
			if (!custom_gpu_score_container.has_value()) continue;
			auto custom_gpu_score = custom_gpu_score_container.value();

			for (std::size_t family_index = 0; family_index < gpu_item.families().size(); ++family_index)
			{
				auto& family = gpu_item.families()[family_index];

				auto additional_work_types = family.job_types ^ settings.type;
				if ((additional_work_types & settings.type).any()) continue;

				std::optional<float> custom_score_container = settings.family_scorer ? settings.family_scorer(family) : std::optional<float>(0.f);
				if (!custom_score_container.has_value()) continue;
				auto custom_score = custom_gpu_score + custom_score_container.value();

				float score = 0.f;
				{
					auto massive_score_addition = _thread_family_count;

					score -= additional_work_types.count() / static_cast<float>(gpu_job_type::size);

					if (is_occupied(family.threads[0])) score -= massive_score_addition;

					auto old_job_count = is_persistent_job ? family.persistent_job_count : family.job_count;
					score -= old_job_count;
					if (old_job_count > family.threads.size()) score -= (old_job_count - family.threads.size()) * (massive_score_addition * 2);
				}

				constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_job_type::size);
				bool is_better = score > best_family_score + very_small_score_difference;
				if (best_family_score - score < very_small_score_difference) is_better = is_better || custom_score > best_family_score_custom;

				if (is_better)
				{
					best_gpu = &gpu_item;
					best_family_index = best_family_index;
					best_family_score = score;
					best_family_score_custom = custom_score;
				}
			}
		}

		if (best_gpu == nullptr)
		{
			throw std::runtime_error("The machine's GPUs could not perform a required job because of the type of work it requires.");
		}

		return { best_gpu, best_family_index };
	}

	auto gpu_manager::find_job_thread_in_family(const gpu_job_settings& settings, bool is_persistent_job, const gpu_thread_family& family) -> std::size_t
	{
		static auto is_occupied_persistent = [](gpu_thread a) { return a.persistent_job_count > 0; };
		static auto is_occupied_nonpersistent = [](gpu_thread a) { return a.job_count > 0; };
		auto is_occupied = is_persistent_job ? is_occupied_persistent : is_occupied_nonpersistent;

		std::size_t thread_index;
		{
			auto high_priority_thread_is_free = !is_occupied(family.threads[0]);
			auto job_count = is_persistent_job ? family.persistent_job_count : family.job_count;
			auto normal_priority_persistent_job_count = family.persistent_job_count + (high_priority_thread_is_free ? 0 : -1);

			bool use_high_priority_thread = settings.priority == gpu_job_priority::high;
			if (job_count == family.threads.size() - 1) use_high_priority_thread = true; // Use high priority if all normal-priority are being used
			if (is_occupied(family.threads[0])) use_high_priority_thread = false;

			if (use_high_priority_thread) thread_index = 0;
			else if (job_count >= family.threads.size()) // No free threads
			{
				std::size_t best_thread_index = -1;
				float best_thread_score = std::numeric_limits<float>::lowest();

				for (std::size_t i = 0; i < family.threads.size(); ++i)
				{
					auto& thread = family.threads[i];

					auto thread_job_count = is_persistent_job ? thread.persistent_job_count : thread.job_count;

					float score = 0;
					{
						score -= thread_job_count;
						if (settings.priority != gpu_job_priority::high && i == 0) score -= 0.1f;
					}

					if (score > best_thread_score)
					{
						best_thread_index = i;
						best_thread_score = score;
					}
				}

				thread_index = best_thread_index;
			}
			else
			{
				thread_index = 1 + normal_priority_persistent_job_count;

				if (!is_persistent_job)
				{
					for (; thread_index < family.threads.size(); ++thread_index)
					{
						if (is_occupied(family.threads[thread_index])) continue;
						break;
					}
				}
			}
		}

		return thread_index;
	}

	/******************************** constructors ********************************/

	gpu_manager::gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance) : _thread_family_count(0)
	{
		auto instance = Private::to_vulkan(vulkan_instance);

		{
			auto physicalDevices = Private::get_size_and_vector<uint32_t, VkPhysicalDevice>(
				[instance](uint32_t* size, VkPhysicalDevice* data)
				{
					auto result = vkEnumeratePhysicalDevices(instance, size, data);
					switch (result)
					{
					case VK_SUCCESS:
					case VK_INCOMPLETE:
						break;
					default: throw std::runtime_error("Could not get the machine's graphics card");
					}
				}
			);

			_gpus.reserve(physicalDevices.size());
			for (auto& physical_device : physicalDevices)
			{
				auto vulkan_physical_device = Private::from_vulkan(physical_device);
				gpu new_gpu(vulkan_instance, vulkan_physical_device);
				_thread_family_count += new_gpu.families().size();
				_gpus.push_back(std::move(new_gpu));
			}
		}
	}
}
