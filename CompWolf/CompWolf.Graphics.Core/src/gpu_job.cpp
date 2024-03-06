#include "pch.h"
#include "gpu"

namespace CompWolf::Graphics
{
	gpu_job::gpu_job(gpu& device_in, size_t family_index_in, size_t thread_index_in, bool is_persistent) :
		_device(&device_in),
		_family_index(family_index_in),
		_thread_index(thread_index_in),
		_is_persistent(is_persistent)
	{
		++family().job_count;
		++thread().job_count;
		if (is_persistent)
		{
			++family().persistent_job_count;
			++thread().persistent_job_count;
		}
	}
	gpu_job::~gpu_job()
	{
		if (empty()) return;

		--family().job_count;
		--thread().job_count;
		if (is_persistent())
		{
			--family().persistent_job_count;
			--thread().persistent_job_count;
		}
	}

	gpu_job::gpu_job(gpu_job&& other)
	{
		_device = std::move(other._device);
		_family_index = std::move(other._family_index);
		_thread_index = std::move(other._thread_index);

		other._device = nullptr;
	}
	auto gpu_job::operator=(gpu_job&& other) -> gpu_job&
	{
		_device = std::move(other._device);
		_family_index = std::move(other._family_index);
		_thread_index = std::move(other._thread_index);

		other._device = nullptr;

		return *this;
	}
}
