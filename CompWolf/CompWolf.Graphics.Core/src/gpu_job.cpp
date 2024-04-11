#include "pch.h"
#include "gpus"

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	gpu_job::gpu_job(gpu_connection& device_in, std::size_t family_index_in, std::size_t thread_index_in) :
		_device(&device_in),
		_family_index(family_index_in),
		_thread_index(thread_index_in)
	{
		++family().job_count;
		++thread().job_count;
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_job::free() noexcept
	{
		if (empty()) return;

		--family().job_count;
		--thread().job_count;

		_device = nullptr;
	}
}
