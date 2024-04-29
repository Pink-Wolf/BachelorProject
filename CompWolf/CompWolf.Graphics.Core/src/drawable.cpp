#include "pch.h"
#include "drawables"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include <functional>

namespace CompWolf::Graphics
{
	/******************************** event handler ********************************/

	void base_drawable::draw_program_code(draw_code_parameters& args)
	{
		auto command = Private::to_vulkan(args.command);
		auto logicDevice = Private::to_vulkan(target_window().gpu().vulkan_device());

		{
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, Private::to_vulkan(material().vulkan_pipeline()));

			auto& [width, height] = target_window().pixel_size().value();
			VkViewport viewport{
				.x = .0f,
				.y = .0f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = .0f,
				.maxDepth = 1.f,
			};
			vkCmdSetViewport(command, 0, 1, &viewport);

			VkRect2D renderArea{
				.offset = {0, 0},
				.extent = {
					.width = static_cast<uint32_t>(width),
					.height = static_cast<uint32_t>(height),
				},
			};
			vkCmdSetScissor(command, 0, 1, &renderArea);
		}
		{
			auto vkBuffer = Private::to_vulkan(_inputs->vulkan_buffer());
			static VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(command, 0, 1, &vkBuffer, offsets);
		}
		for (size_t i = 0; i < _fields.size(); ++i)
		{
			auto& memory = *_fields[i];
			auto index = material().field_indices()[i];

			Private::gpu_memory_bind_data bind_data{
				.commandBuffer = command,
				.descriptorSet = Private::to_vulkan(material().vulkan_descriptor_sets()[args.frame_index]),
				.bindingIndex = static_cast<uint32_t>(index),
			};

			memory.bind_to_shader(&bind_data);

			vkCmdBindDescriptorSets(command
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, Private::to_vulkan(material().vulkan_pipeline_layout())
				, 0
				, 1
				, &bind_data.descriptorSet
				, 0
				, nullptr
			);
		}
		{
			auto indexBuffer = Private::to_vulkan(_indices->vulkan_buffer());

			vkCmdBindIndexBuffer(command, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		vkCmdDrawIndexed(command, static_cast<uint32_t>(_indices->size()), 1, 0, 0, 0);
	}

	/******************************** constructors ********************************/

	base_drawable::base_drawable(window& window, window_specific_material* material
		, gpu_index_buffer* indices
		, Private::base_gpu_vkBuffer_buffer* inputs
		, std::vector<base_gpu_buffer*> fields
	) : window_user(window)
		, _material(material)
		, _indices(indices)
		, _inputs(inputs)
		, _fields(fields)
	{
		_draw_code_key = window.add_draw_code(std::bind_front(&base_drawable::draw_program_code, this));
	}

	/******************************** CompWolf::freeable ********************************/

	void base_drawable::free() noexcept
	{
		if (empty()) return;

		target_window().remove_draw_code(_draw_code_key);

		set_window(nullptr);
	}
}
