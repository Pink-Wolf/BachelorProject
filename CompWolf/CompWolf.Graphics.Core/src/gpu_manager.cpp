#include "pch.h"
#include "gpus"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>
#include <map>
#include <algorithm>
#include <limits>

namespace CompWolf::Graphics
{
	/******************************** other methods ********************************/

	auto gpu_manager::new_job(gpu_job_settings settings) -> gpu_job
	{
		auto [gpu_pointer, family_index] = find_job_family(settings);
		auto& gpu_device = *gpu_pointer; auto& family = gpu_device.families()[family_index];

		auto thread_index = find_job_thread_in_family(settings, family);
		auto& thread = family.threads[thread_index];

		return gpu_job(gpu_device, family_index, thread_index);
	}

	auto gpu_manager::find_job_family(const gpu_job_settings& settings) -> std::pair<gpu_connection*, std::size_t>
	{
		gpu_connection* best_gpu = nullptr;
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
					score -= additional_work_types.count() / static_cast<float>(gpu_job_type::size);

					score -= static_cast<float>(family.job_count / family.threads.size()); // Truncation of "job-count / thread-count" is intentional
				}

				constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_job_type::size);
				bool is_better = score > best_family_score + very_small_score_difference;
				if (best_family_score - score < very_small_score_difference) is_better = is_better || custom_score > best_family_score_custom; // use custom_score if scores are same

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

	auto gpu_manager::find_job_thread_in_family(const gpu_job_settings& settings, const gpu_thread_family& family) -> std::size_t
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

	/******************************** constructors ********************************/

	gpu_manager::gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance) : _thread_count(0)
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
				gpu_connection new_gpu(vulkan_instance, vulkan_physical_device);
				for (auto& family : new_gpu.families()) _thread_count += family.threads.size();
				_gpus.push_back(std::move(new_gpu));
			}
		}
	}
}
