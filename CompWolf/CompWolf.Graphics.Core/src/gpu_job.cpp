#include "pch.h"
#include "gpu"

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_job::gpu_job(gpu_connection& device_in, std::size_t family_index_in, std::size_t thread_index_in, bool is_persistent) :
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

	/******************************** CompWolf::freeable ********************************/

	void gpu_job::free() noexcept
	{
		if (empty()) return;

		--family().job_count;
		--thread().job_count;
		if (is_persistent())
		{
			--family().persistent_job_count;
			--thread().persistent_job_count;
		}

		_device = nullptr;
	}
}
