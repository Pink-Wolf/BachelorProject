#include "pch.h"
#include "drawables"

#include "compwolf_vulkan.hpp"
#include "gpus"
#include <stdexcept>

namespace CompWolf::Graphics::Private
{
	/******************************** constructors ********************************/

	gpu_specific_material::gpu_specific_material(draw_material_data& data, gpu_connection& in_gpu)
		: _gpu(&in_gpu)
		, _layout_descriptor(nullptr), _layout(nullptr)
	{
		auto logicDevice = Private::to_vulkan(gpu().vulkan_device());

		try
		{
			VkDescriptorSetLayout uniformLayout;
			{
				std::vector<VkDescriptorSetLayoutBinding> uniformBindings;
				uniformBindings.reserve(data.field_types.size() * 2);
				for (std::size_t i = 0; i < data.field_types.size(); ++i)
				{
					VkDescriptorSetLayoutBinding layoutBinding{
						.binding = static_cast<uint32_t>(data.field_indices[i]),
						.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						.descriptorCount = 1,
					};

					if (data.field_types[i] == draw_material_data::field_type::image)
						layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

					if (data.field_is_input_field[i]) layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
					if (data.field_is_pixel_field[i]) layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

					uniformBindings.emplace_back(std::move(layoutBinding));
				}

				VkDescriptorSetLayoutCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.bindingCount = static_cast<uint32_t>(uniformBindings.size()),
					.pBindings = uniformBindings.data(),
				};

				auto result = vkCreateDescriptorSetLayout(logicDevice, &createInfo, nullptr, &uniformLayout);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up uniform fields for pipeline's vertex shader.");
				}

				_layout_descriptor = Private::from_vulkan(uniformLayout);
			}

			VkPipelineLayout pipelineLayout;
			{
				VkPipelineLayoutCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.setLayoutCount = 1,
					.pSetLayouts = &uniformLayout,
					.pushConstantRangeCount = 0,
				};

				auto result = vkCreatePipelineLayout(logicDevice, &createInfo, nullptr, &pipelineLayout);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"uniform\" values for shader in pipeline.");
				}

				_layout = Private::from_vulkan(pipelineLayout);
			}
		}
		catch (...)
		{
			free();
			throw;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_specific_material::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = to_vulkan(gpu().vulkan_device());
		if (_layout) vkDestroyPipelineLayout(logicDevice, Private::to_vulkan(_layout), nullptr);
		if (_layout_descriptor) vkDestroyDescriptorSetLayout(logicDevice, Private::to_vulkan(_layout_descriptor), nullptr);

		_gpu = nullptr;
	}
}
