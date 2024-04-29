#include "pch.h"
#include "drawables"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include "shader_field_type.hpp"
#include "window"
#include <stdexcept>
#include <functional>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	window_specific_material::window_specific_material
	(Private::draw_material_data& data, Private::gpu_specific_material& gpu_data, window& window)
		: window_user(window), _gpu_data(&gpu_data)
		, _descriptor_pool(nullptr), _pipeline(nullptr)
		, _field_indices(&data.field_indices)
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto& surface_format = *Private::to_private(target_window().surface().vulkan_format());
		auto renderpass = Private::to_vulkan(target_window().surface().vulkan_render_pass());
		auto& frames = target_window().swapchain().frames();
		auto vkPipelineLayout = Private::to_vulkan(vulkan_pipeline_layout());

		try
		{
			std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfo;
			{
				VkPipelineShaderStageCreateInfo vertexCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = Private::to_vulkan(data.input_shader->vulkan_shader(device())),
					.pName = "main",
				};

				VkPipelineShaderStageCreateInfo fragCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = Private::to_vulkan(data.pixel_shader->vulkan_shader(device())),
					.pName = "main",
				};

				stageCreateInfo = { std::move(vertexCreateInfo), std::move(fragCreateInfo) };
			}

			VkVertexInputBindingDescription bindingDescriptions{
				.binding = static_cast<uint32_t>(0),
				.stride = static_cast<uint32_t>(data.input_stride),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
			};
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
			{
				attributeDescriptions.reserve(data.input_types->size());
				for (uint32_t i = 0; i < data.input_types->size(); ++i)
				{
					auto& info = *Private::to_private(data.input_types->at(i));
					auto offset = data.input_offsets->at(i);
					attributeDescriptions.emplace_back(VkVertexInputAttributeDescription
						{
							.location = i,
							.binding = 0,
							.format = info.format,
							.offset = static_cast<uint32_t>(offset),
						}
					);
				}
			}
			VkPipelineVertexInputStateCreateInfo inputCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 1,
				.pVertexBindingDescriptions = &bindingDescriptions,
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
				.pVertexAttributeDescriptions = attributeDescriptions.data(),
			};
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE,
			};

			std::vector<VkDynamicState> dynamicStates{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};
			VkPipelineDynamicStateCreateInfo dynamic_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
				.pDynamicStates = dynamicStates.data(),
			};

			auto descriptorSize = 2 * static_cast<uint32_t>(frames.size());
			VkDescriptorPool descriptorPool;
			{
				std::vector<VkDescriptorPoolSize> poolSizes{
					{
					.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = descriptorSize,
					},
					{
					.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = descriptorSize,
					}
				};
				VkDescriptorPoolCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
					.maxSets = descriptorSize,
					.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
					.pPoolSizes = poolSizes.data(),
				};

				auto result = vkCreateDescriptorPool(logicDevice, &createInfo, nullptr, &descriptorPool);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"descriptor pool\" to connect uniform buffers to vertex shader.");
				}

				_descriptor_pool = Private::from_vulkan(descriptorPool);
			}
			std::vector<VkDescriptorSet> descriptorSets;
			{
				std::vector<VkDescriptorSetLayout> descriptorLayouts(descriptorSize, Private::to_vulkan(vulkan_pipeline_layout_descriptor()));

				VkDescriptorSetAllocateInfo allocateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = descriptorPool,
					.descriptorSetCount = descriptorSize,
					.pSetLayouts = descriptorLayouts.data(),
				};

				descriptorSets.resize(descriptorSize);
				auto result = vkAllocateDescriptorSets(logicDevice, &allocateInfo, descriptorSets.data());

				switch (result)
				{
				case VK_SUCCESS: break;
				default: descriptorSets.clear();
					throw std::runtime_error("Could not set up \"descriptor pool\" to connect uniform buffers to vertex shader.");
				}

				_descriptor_sets.reserve(descriptorSets.size());
				for (auto set : descriptorSets) _descriptor_sets.push_back(Private::from_vulkan(set));
			}

			uint32_t width, height;
			{
				auto& size = target_window().pixel_size().value();
				width = static_cast<uint32_t>(size.first);
				height = static_cast<uint32_t>(size.second);
			}
			VkViewport viewport{
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = 0.f,
				.maxDepth = 1.f,
			};
			VkRect2D scissor{
				.offset = {0, 0},
				.extent = {
					.width = width,
					.height = height,
				},
			};
			VkPipelineViewportStateCreateInfo viewportCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.pViewports = &viewport,
				.scissorCount = 1,
				.pScissors = &scissor,
			};

			VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.depthClampEnable = VK_FALSE,
				.rasterizerDiscardEnable = VK_FALSE,
				.polygonMode = VK_POLYGON_MODE_FILL,
				.cullMode = VK_CULL_MODE_BACK_BIT,
				.frontFace = VK_FRONT_FACE_CLOCKWISE,
				.depthBiasEnable = VK_FALSE,
				.lineWidth = 1.f,
			};

			VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
			};

			VkPipelineColorBlendAttachmentState blendState{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};
			VkPipelineColorBlendStateCreateInfo blendCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = 1,
				.pAttachments = &blendState,
			};

			VkPipeline pipeline;
			{
				VkGraphicsPipelineCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.stageCount = static_cast<uint32_t>(stageCreateInfo.size()),
					.pStages = stageCreateInfo.data(),
					.pVertexInputState = &inputCreateInfo,
					.pInputAssemblyState = &inputAssemblyCreateInfo,
					.pViewportState = &viewportCreateInfo,
					.pRasterizationState = &rasterizationCreateInfo,
					.pMultisampleState = &multisampleCreateInfo,
					.pDepthStencilState = nullptr,
					.pColorBlendState = &blendCreateInfo,
					.pDynamicState = &dynamic_create_info,
					.layout = vkPipelineLayout,
					.renderPass = renderpass,
					.subpass = 0,
					.basePipelineHandle = nullptr,
					.basePipelineIndex = -1,
				};

				auto result = vkCreateGraphicsPipelines(logicDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"render pass\" for pipeline.");
				}
			}
			_pipeline = Private::from_vulkan(pipeline);

			_frame_buffers.reserve(frames.size());
			for (std::size_t frame_iterator = 0; frame_iterator < frames.size(); ++frame_iterator)
			{
				VkImageView swapchainImage = Private::to_vulkan(frames[frame_iterator].image);

				VkFramebufferCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass = renderpass,
					.attachmentCount = 1,
					.pAttachments = &swapchainImage,
					.width = width,
					.height = height,
					.layers = 1,
				};

				VkFramebuffer framebuffer;
				auto result = vkCreateFramebuffer(logicDevice, &create_info, nullptr, &framebuffer);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"framebuffer\" for pipeline.");
				}

				_frame_buffers.push_back(Private::from_vulkan(framebuffer));
			}
		}
		catch (...)
		{
			free();
			throw;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void window_specific_material::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(target_window().device().vulkan_device());

		vkDeviceWaitIdle(logicDevice);

		for (auto& framebuffer : _frame_buffers) vkDestroyFramebuffer(logicDevice, Private::to_vulkan(framebuffer), nullptr);
		_frame_buffers.clear();
		if (_pipeline) vkDestroyPipeline(logicDevice, Private::to_vulkan(_pipeline), nullptr);

		if (_descriptor_pool) vkDestroyDescriptorPool(logicDevice, Private::to_vulkan(_descriptor_pool), nullptr);

		set_window(nullptr);
	}
}
