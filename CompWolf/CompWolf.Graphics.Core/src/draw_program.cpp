#include "pch.h"
#include "draw_program"

#include "compwolf_vulkan.hpp"

namespace CompWolf::Graphics
{
	/******************************** getters ********************************/

	auto draw_program::window_program(window& target) noexcept -> Private::draw_window_program&
	{
		return _window_commands.try_emplace(&target, _data, target).first->second;
	}

	/******************************** other methods ********************************/

	void Private::draw_frame_program::gpu_code(const gpu_program_input& args)
	{
		auto commandBuffer = Private::to_vulkan(args.command);
		auto& frame = target_window().swapchain().frames()[frame_index()];

		auto& clear_color = _data->settings.background;
		VkClearValue clearColor = { {{clear_color.x(), clear_color.y(), clear_color.z()}} };

		uint32_t width, height;
		{
			auto size = target_window().pixel_size().value();
			width = static_cast<uint32_t>(size.first);
			height = static_cast<uint32_t>(size.second);
		}

		VkRenderPassBeginInfo renderpassInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = Private::to_vulkan(target_window().surface().render_pass()),
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
			draw_program_input draw_args{
				{ args },
				&target_window(),
				frame_index(),
			};
			_data->code(draw_args);
		}
		vkCmdEndRenderPass(commandBuffer);
	}

	/******************************** constructors ********************************/

	Private::draw_window_program::draw_window_program(draw_program_data& data, window& target)
		: _target_window(&target), _data(&data)
	{
		auto& frames = _target_window->swapchain().frames();
		_frame_programs.reserve(frames.size());
		for (std::size_t i = 0; i < frames.size(); ++i)
		{
			_frame_programs.emplace_back(data, target, i);
		}
	}

	Private::draw_frame_program::draw_frame_program(draw_program_data& data, window& target, std::size_t frame_index)
		: _target_window(&target), _data(&data), _frame_index(frame_index)
		, _program(target.device()
			, target.swapchain().frames()[frame_index].pool
			, std::bind_front(&draw_frame_program::gpu_code, this)
		)
	{

	}
}
