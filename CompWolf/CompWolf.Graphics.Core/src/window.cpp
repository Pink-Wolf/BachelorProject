#include "pch.h"
#include "windows"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include "graphics"
#include <compwolf_utility>
#include "shaders"
#include <functional>

namespace CompWolf
{
	/******************************** event handlers ********************************/

	void window::frame_drawing_program_code(std::size_t frame_index, const gpu_program_code_parameters& args)
	{
		auto commandBuffer = Private::to_vulkan(args.command);
		auto& frame = swapchain().frames()[frame_index];

		auto clear_color = opaque_color(.2_scolor, .2_scolor, .2_scolor);
		VkClearValue clearColor = {
			{{
				static_cast<float>(clear_color.x()),
				static_cast<float>(clear_color.y()),
				static_cast<float>(clear_color.z())
			}}
		};

		uint32_t width, height;
		{
			auto& size = pixel_size().value();
			width = static_cast<uint32_t>(size.x());
			height = static_cast<uint32_t>(size.y());
		}

		VkRenderPassBeginInfo renderpassInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = Private::to_vulkan(surface().vulkan_render_pass()),
			.framebuffer = Private::to_vulkan(frame.frame_buffer),
			.renderArea = {
				.offset = {0, 0},
				.extent = {
					.width = width,
					.height = height,
				},
			},
			.clearValueCount = 1,
			.pClearValues = &clearColor,
		};

		vkCmdBeginRenderPass(commandBuffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			draw_code_parameters drawing_args{
				{ args },
				this,
				&frame,
				frame_index,
			};
			_drawing_code.invoke(drawing_args);
		}
		vkCmdEndRenderPass(commandBuffer);
	}

	/******************************** modifiers ********************************/

	void window::update_image()
	{
		// draw
		{
			if (_frame_drawing_programs.empty())
				_frame_drawing_programs.resize(swapchain().frames().size());

			auto& current_program = _frame_drawing_programs[swapchain().current_frame_index()];
			if (current_program.empty())
			{
				current_program = gpu_program(
					swapchain().current_frame().draw_job,
					std::bind_front(&window::frame_drawing_program_code, this, swapchain().current_frame_index())
				);
			}

			current_program.execute();
		}

		// present
		{
			auto& frame = swapchain().current_frame();
			auto semaphore = Private::to_vulkan(frame.draw_job.last_vulkan_semaphore());
			auto vkSwapchain = Private::to_vulkan(swapchain().vulkan_swapchain());
			auto frame_index = static_cast<uint32_t>(swapchain().current_frame_index());
			auto queue = Private::to_vulkan(frame.draw_job.thread().queue);

			VkPresentInfoKHR presentInfo{
				.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = (semaphore == nullptr) ? 0_uint32 : 1_uint32,
				.pWaitSemaphores = &semaphore,
				.swapchainCount = 1,
				.pSwapchains = &vkSwapchain,
				.pImageIndices = &frame_index,
			};

			vkQueuePresentKHR(queue, &presentInfo);
		}

		// handle events
		for (auto& event : _draw_events)
		{
			event.second(*this);
		}
		_draw_events.clear();

		// get next frame
		swapchain().to_next_frame();
	}

	void window::set_pixel_size(int width, int height)
	{
		window_rebuild_surface_parameters event_args{
			.surface = &_surface,
			.swapchain = &_swapchain,
		};
		rebuilding_surface(event_args);
		auto& old_gpu = gpu();

		_swapchain.free();
		_surface.free();

		_settings.pixel_size = { width, height };

		_surface = window_surface(_settings, _glfw_window, nullptr, &old_gpu);
		_swapchain = window_swapchain(_settings, _glfw_window, _surface);

		event_args.surface = &_surface;
		event_args.swapchain = &_swapchain;
		rebuild_surface(event_args);

		_pixel_size.set_value(int2({ width, height }));
	}

	/******************************** constructors ********************************/

	window::window(graphics_environment* environment, gpu_connection* gpu, window_settings settings) : _settings(settings)
	{
		if (_settings.pixel_size.x() <= 0 || _settings.pixel_size.y() <= 0)
			_settings.pixel_size = int2({ 480, 480 });

		if (_settings.name.empty())
			_settings.name = "Window";

		try
		{
			_pixel_size.set_value(_settings.pixel_size);

			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				auto glfwWindow = glfwCreateWindow(_pixel_size.value().x(), _pixel_size.value().y()
					, _settings.name.data()
					, nullptr, nullptr);
				if (!glfwWindow)
				{
					auto error_code = glfwGetError(NULL);
					switch (error_code)
					{
					case GLFW_API_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right API.");
					case GLFW_VERSION_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right version of OpenGL or openGL ES.");
					case GLFW_FORMAT_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right pixel format.");
					default: throw std::runtime_error("Could not create a window; unknown error from glfw-function.");
					}
				}
				_glfw_window = Private::from_glfw(glfwWindow);

				glfwSetWindowUserPointer(glfwWindow, this);

				glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow) {
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->close();
					});

				glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->_draw_events.erase(draw_event_type::resize);
					this_window->_draw_events.emplace(draw_event_type::resize, [width, height](window& w)->void { w.set_pixel_size(width, height); });
					});

				glfwSetInputMode(glfwWindow, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
				_inputs = input_manager(settings, _glfw_window);
				glfwSetKeyCallback(glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->inputs().on_glfw_input(key, action, mods);
					});
			}

			_surface = window_surface(_settings, _glfw_window, environment, gpu);
			_swapchain = window_swapchain(_settings, _glfw_window, _surface);
		}
		catch (...)
		{
			free(); // Make sure to release data

			throw;
		}

	}

	/******************************** CompWolf::freeable ********************************/

	void window::free() noexcept
	{
		if (empty()) return;

		freeing();

		_frame_drawing_programs.clear();

		_swapchain.free();
		_surface.free();

		auto glfwWindow = Private::to_glfw(_glfw_window);
		glfwDestroyWindow(glfwWindow);
		_glfw_window = nullptr;

		freed();
	}
}
