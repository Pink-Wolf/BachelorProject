#include "pch.h"
#include "window"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <mutex>
#include <algorithm>
#include <optional>

namespace CompWolf::Graphics
{
	window_surface::window_surface(graphics_environment& environment, Private::glfw_window& window)
	{
		auto instance = Private::to_vulkan(environment.vulkan_instance());
		auto glfwWindow = Private::to_glfw(window);

		try
		{
			// Surface
			VkSurfaceKHR surface;
			{
				auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not get a window's surface; unknown error from glfw/vulkan-function.");
				}

				_vulkan_surface = Private::from_vulkan(surface);
			}

			// Job
			Private::surface_format_info job_info;
			{
				std::unordered_map<Private::vulkan_physical_device, Private::surface_format_info> device_infos;
				_draw_present_job = environment.gpus().new_persistent_job(gpu_job_settings{
					.type = {gpu_job_type::present, gpu_job_type::draw},
					.priority = gpu_job_priority::high,
					.gpu_scorer = Private::evaluate_gpu_for_present(device_infos, _vulkan_surface),
					});
				job_info = std::move(device_infos[_draw_present_job.device().vulkan_physical_device()]);
			}
			auto& job_family = _draw_present_job.family();
			auto& gpu_device = _draw_present_job.device();
			auto device = Private::to_vulkan(gpu_device.vulkan_device());

			_format = Private::from_private(new Private::surface_format_info(std::move(job_info)));
			set_device(gpu_device);
		}
		catch (...)
		{
			destroy(); // Make sure to release data

			throw;
		}

	}
	window_surface::window_surface(window_surface&& other) noexcept
	{
		set_device(other.device());
		_vulkan_surface = std::move(other._vulkan_surface);
		_draw_present_job = std::move(other._draw_present_job);
		_format = std::move(other._format);

		other._vulkan_surface = nullptr;
	}
	auto window_surface::operator=(window_surface&& other) noexcept -> window_surface&
	{
		set_device(other.device());
		_vulkan_surface = std::move(other._vulkan_surface);
		_draw_present_job = std::move(other._draw_present_job);
		_format = std::move(other._format);

		other._vulkan_surface = nullptr;

		return *this;
	}
	window_surface::~window_surface()
	{
		destroy();
	}

	void window_surface::destroy() noexcept
	{
		if (is_destroyed()) return;

		auto instance = Private::to_vulkan(device().vulkan_instance());
		auto vulkan_device = Private::to_vulkan(device().vulkan_device());

		if (_format) delete Private::to_private(_format);
		if (_vulkan_surface) vkDestroySurfaceKHR(instance, Private::to_vulkan(_vulkan_surface), nullptr);

		_vulkan_surface = nullptr;
	}
}
