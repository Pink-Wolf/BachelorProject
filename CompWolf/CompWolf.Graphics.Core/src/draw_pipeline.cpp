#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include "shader_field_type.hpp"
#include <stdexcept>
#include <algorithm>
#include <ranges>

namespace CompWolf::Graphics
{
	/******************************** event handlers ********************************/

	void window_specific_pipeline::on_swapchain_rebuild(
		const event<window_rebuild_swapchain_parameter>&,
		window_rebuild_swapchain_parameter& args
	) {
		auto pipeline_data = _pipeline_data;
		auto gpu_data = _gpu_data;
		auto& target = target_window();

		free();

		_pipeline_data = pipeline_data;
		_gpu_data = gpu_data;
		set_target_window(target);


		setup();
	}

	/******************************** constructors ********************************/

	Private::gpu_specific_pipeline::gpu_specific_pipeline(gpu& gpu_device, const draw_pipeline_data& data)
	{
		_device = &gpu_device;
		auto device = Private::to_vulkan(gpu_device.vulkan_device());

		VkPipelineLayout pipelineLayout;
		{
			VkPipelineLayoutCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.setLayoutCount = 0,
				.pushConstantRangeCount = 0,
			};

			auto result = vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not set up \"uniform\" values for shader in pipeline.");
			}
		}
		_layout = Private::from_vulkan(pipelineLayout);
	}

	void window_specific_pipeline::setup()
	{
		auto& gpu_device = target_window().device();
		auto& thread_family = target_window().draw_present_job().family();
		auto logicDevice = Private::to_vulkan(gpu_device.vulkan_device());
		auto& surface_format = *Private::to_private(target_window().surface().format());
		auto& frames = target_window().swapchain().frames();

		auto vkPipelineLayout = Private::to_vulkan(_gpu_data->layout());

		try
		{
			std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfo;
			{
				VkPipelineShaderStageCreateInfo vertexCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = Private::to_vulkan(_pipeline_data->vertex_shader->shader_module(gpu_device)),
					.pName = "main",
				};

				VkPipelineShaderStageCreateInfo fragCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = Private::to_vulkan(_pipeline_data->fragment_shader->shader_module(gpu_device)),
					.pName = "main",
				};

				stageCreateInfo = { std::move(vertexCreateInfo), std::move(fragCreateInfo) };
			}

			VkVertexInputBindingDescription bindingDescription{
				.binding = 0,
				.stride = static_cast<uint32_t>(_pipeline_data->input_stride),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
			};
			std::vector<VkVertexInputAttributeDescription> inputAttributes;
			inputAttributes.reserve(_pipeline_data->input_types->size());
			for (uint32_t i = 0; i < _pipeline_data->input_types->size(); ++i)
			{
				auto& info = *Private::to_private(_pipeline_data->input_types->operator[](i));
				auto offset = (*_pipeline_data->input_offsets)[i];
				inputAttributes.push_back(VkVertexInputAttributeDescription
					{
						.location = i,
						.binding = 0,
						.format = info.format,
						.offset = static_cast<uint32_t>(offset),
					}
				);
			}

			VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 1,
				.pVertexBindingDescriptions = &bindingDescription,
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributes.size()),
				.pVertexAttributeDescriptions = inputAttributes.data(),
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

			uint32_t width, height;
			{
				auto size = target_window().pixel_size().value();
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

			VkRenderPass renderpass;
			{
				VkAttachmentDescription colorAttachment{
					.format = surface_format.format.format,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				};

				VkAttachmentReference colorAttachmentReference{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				};
				VkSubpassDescription subpass{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.colorAttachmentCount = 1,
					.pColorAttachments = &colorAttachmentReference,
				};

				VkSubpassDependency dependency{
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = 0,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				};

				VkRenderPassCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
					.attachmentCount = 1,
					.pAttachments = &colorAttachment,
					.subpassCount = 1,
					.pSubpasses = &subpass,
					.dependencyCount = 1,
					.pDependencies = &dependency,
				};

				auto result = vkCreateRenderPass(logicDevice, &createInfo, nullptr, &renderpass);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"render pass\" for shader in pipeline.");
				}
			}
			_render_pass = Private::from_vulkan(renderpass);

			VkPipeline pipeline;
			{
				VkGraphicsPipelineCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.stageCount = static_cast<uint32_t>(stageCreateInfo.size()),
					.pStages = stageCreateInfo.data(),
					.pVertexInputState = &vertexInputCreateInfo,
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

	/******************************** getters ********************************/

	auto Private::base_draw_pipeline::window_data(window& target_window) const noexcept -> const window_specific_pipeline&
	{
		// Get data if it does exist
		{
			auto iterator = _window_data.find(&target_window);
			if (iterator != _window_data.end()) return iterator->second;
		}
		// Create data if it does not exist
		{
			auto& gpu_device = target_window.device();
			auto& gpu_data = _gpu_data.try_emplace(&gpu_device, gpu_device, _pipeline_data).first->second;
			auto& window_data = _window_data.try_emplace(&target_window, target_window, _pipeline_data, gpu_data).first->second;
			return window_data;
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void Private::gpu_specific_pipeline::free() noexcept
	{
		if (empty()) return;

		auto vulkan_device = to_vulkan(device().vulkan_device());
		if (_layout) vkDestroyPipelineLayout(vulkan_device, Private::to_vulkan(_layout), nullptr);

		_device = nullptr;
	}

	void window_specific_pipeline::free() noexcept
	{
		if (empty()) return;

		freeing();

		auto logicDevice = Private::to_vulkan(target_window().device().vulkan_device());

		vkDeviceWaitIdle(logicDevice);

		for (auto& framebuffer : _frame_buffers) vkDestroyFramebuffer(logicDevice, Private::to_vulkan(framebuffer), nullptr);
		_frame_buffers.clear();
		if (_pipeline) vkDestroyPipeline(logicDevice, Private::to_vulkan(_pipeline), nullptr);
		if (_render_pass) vkDestroyRenderPass(logicDevice, Private::to_vulkan(_render_pass), nullptr);

		set_target_window(nullptr);
	}
}
