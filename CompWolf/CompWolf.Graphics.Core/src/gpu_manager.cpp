#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <ranges>
#include <map>
#include <algorithm>
#include <limits>

namespace CompWolf::Graphics
{
	gpu_manager::gpu_manager(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance) : _thread_family_count(0), persistent_jobs(), closed_jobs_in_persistent_jobs(0)
	{
		auto instance = Private::to_vulkan(vulkan_instance);

		{
			auto physical_devices = Private::get_size_and_vector<VkInstance, uint32_t, VkPhysicalDevice, VkResult>(vkEnumeratePhysicalDevices, instance, [](VkResult result)
				{
					switch (result)
					{
					case VK_SUCCESS:
					case VK_INCOMPLETE:
						break;
					default: throw std::runtime_error("Could not get the machine's graphics card");
					}
				});

			_gpus.reserve(physical_devices.size());
			for (auto& physical_device : physical_devices)
			{
				auto vulkan_device = Private::from_vulkan(physical_device);
				gpu new_gpu(vulkan_instance, vulkan_device);
				_thread_family_count += new_gpu.families().size();
				_gpus.push_back(std::move(new_gpu));
			}
		}

		for (auto& persistent_job : settings.persistent_jobs)
		{
			new_persistent_job(persistent_job);
		}
	}

	auto gpu_manager::new_persistent_job(gpu_job_settings settings) -> persistent_job_key
	{
		auto family = find_job_family(settings, true);
		auto index_in_family = find_job_thread_in_family(settings, true, family);
		auto thread = family.threads[index_in_family];
		gpu_thread_position position{
			.family = family,
			.index = index_in_family,
		};

		occupy_thread_for_job(settings, true, family, index_in_family);

		auto job_index = persistent_jobs.size();
		if (closed_jobs_in_persistent_jobs == 0)
		{
			persistent_jobs.push_back(std::move(position));
		}
		else
		{
			do { --job_index; } while (persistent_jobs[job_index].has_value());

			persistent_jobs[job_index] = std::move(position);
			--closed_jobs_in_persistent_jobs;
		}

		return job_index;
	}
	void gpu_manager::close_persistent_job(persistent_job_key key)
	{
		persistent_jobs[key] = std::nullopt;
		++closed_jobs_in_persistent_jobs;
	}

	auto gpu_manager::find_job_family(const gpu_job_settings& settings, bool is_persistent_job) -> gpu_thread_family&
	{
		static auto is_occupied_persistent = [](gpu_thread a) { return a.persistent_job_count > 0; };
		static auto is_occupied_nonpersistent = [](gpu_thread a) { return a.job_count > 0; };
		auto is_occupied = is_persistent_job ? is_occupied_persistent : is_occupied_nonpersistent;

		gpu_thread_family* best_family = nullptr;
		float best_family_score = std::numeric_limits<float>::lowest();
		for (auto& gpu_item : _gpus)
		{
			auto additional_work_types_for_gpu = gpu_item.work_types() ^ settings.type;
			if ((additional_work_types_for_gpu & settings.type).any()) continue;

			for (auto& family : gpu_item.families())
			{
				auto additional_work_types = family.job_types ^ settings.type;
				if ((additional_work_types & settings.type).any()) continue;

				float score = 0.f;
				{
					auto massive_score_addition = _thread_family_count;

					score -= additional_work_types.count() / static_cast<float>(gpu_job_type::size);

					if (is_occupied(family.threads[0])) score -= massive_score_addition;

					auto old_job_count = is_persistent_job ? family.persistent_job_count : family.job_count;
					score -= old_job_count;
					score -= (old_job_count % family.threads.size()) * (massive_score_addition * 2);
				}

				if (score > best_family_score)
				{
					best_family = &family;
					best_family_score = score;
				}
			}
		}

		if (best_family == nullptr)
		{
			throw std::runtime_error("The machine's GPUs could not perform a required job because of the type of work it requires.");
		}

		return *best_family;
	}
	auto gpu_manager::find_job_thread_in_family(const gpu_job_settings& settings, bool is_persistent_job, const gpu_thread_family& family) -> size_t
	{
		static auto is_occupied_persistent = [](gpu_thread a) { return a.persistent_job_count > 0; };
		static auto is_occupied_nonpersistent = [](gpu_thread a) { return a.job_count > 0; };
		auto is_occupied = is_persistent_job ? is_occupied_persistent : is_occupied_nonpersistent;

		size_t thread_index;
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
				size_t best_thread_index = -1;
				float best_thread_score = std::numeric_limits<float>::lowest();

				for (size_t i = 0; i < family.threads.size(); ++i)
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

	void gpu_manager::occupy_thread_for_job(const gpu_job_settings& settings, bool is_persistent_job, gpu_thread_family& family, size_t index_in_family)
	{
		auto& thread = family.threads[index_in_family];

		if (is_persistent_job) ++family.persistent_job_count;
		++family.job_count;

		if (is_persistent_job) ++thread.persistent_job_count;
		++thread.job_count;

		if (thread.occupation != gpu_thread_occupation::persistent_occupied)
			thread.occupation = is_persistent_job ? gpu_thread_occupation::persistent_occupied : gpu_thread_occupation::occupied;
	}
}
