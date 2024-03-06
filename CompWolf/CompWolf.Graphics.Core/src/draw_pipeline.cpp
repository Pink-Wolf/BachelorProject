#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include "present_device_info.hpp"
#include <stdexcept>
#include <algorithm>

namespace CompWolf::Graphics
{
	void draw_pipeline::setup()
	{
		auto& gpu_device = target_window().device();
		auto& thread_family = target_window().draw_present_job().family();
		auto device = Private::to_vulkan(gpu_device.vulkan_device());
		auto& surface_format = *Private::to_private(target_window().surface().format());
		auto& frames = target_window().swapchain().frames();

		try
		{
			std::vector<VkPipelineShaderStageCreateInfo> stage_create_info;
			{
				VkPipelineShaderStageCreateInfo vertex_create_info{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = Private::to_vulkan(_settings.vertex_shader->shader_module(gpu_device)),
					.pName = "main",
				};

				VkPipelineShaderStageCreateInfo frag_create_info{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = Private::to_vulkan(_settings.fragment_shader->shader_module(gpu_device)),
					.pName = "main",
				};

				stage_create_info = { std::move(vertex_create_info), std::move(frag_create_info) };
			}
			VkPipelineVertexInputStateCreateInfo vertex_input_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 0,
				.pVertexBindingDescriptions = nullptr,
				.vertexAttributeDescriptionCount = 0,
				.pVertexAttributeDescriptions = nullptr,
			};
			VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE,
			};

			std::vector<VkDynamicState> dynamic_states{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};
			VkPipelineDynamicStateCreateInfo dynamic_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
				.pDynamicStates = dynamic_states.data(),
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
			VkPipelineViewportStateCreateInfo viewport_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.pViewports = &viewport,
				.scissorCount = 1,
				.pScissors = &scissor,
			};

			VkPipelineRasterizationStateCreateInfo rasterization_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.depthClampEnable = VK_FALSE,
				.rasterizerDiscardEnable = VK_FALSE,
				.polygonMode = VK_POLYGON_MODE_FILL,
				.cullMode = VK_CULL_MODE_BACK_BIT,
				.frontFace = VK_FRONT_FACE_CLOCKWISE,
				.depthBiasEnable = VK_FALSE,
				.lineWidth = 1.f,
			};

			VkPipelineMultisampleStateCreateInfo multisample_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
			};

			VkPipelineColorBlendAttachmentState blend_state{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};
			VkPipelineColorBlendStateCreateInfo blend_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = 1,
				.pAttachments = &blend_state,
			};


			VkPipelineLayout pipeline_layout;
			{
				VkPipelineLayoutCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.setLayoutCount = 0,
					.pushConstantRangeCount = 0,
				};

				auto result = vkCreatePipelineLayout(device, &create_info, nullptr, &pipeline_layout);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"uniform\" values for shader in pipeline.");
				}
			}
			_layout = Private::from_vulkan(pipeline_layout);

			VkRenderPass renderpass;
			{
				VkAttachmentDescription color_attachment{
					.format = surface_format.format.format,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				};

				VkAttachmentReference color_attachment_reference{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				};
				VkSubpassDescription subpass{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.colorAttachmentCount = 1,
					.pColorAttachments = &color_attachment_reference,
				};

				VkSubpassDependency dependency{
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = 0,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				};

				VkRenderPassCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
					.attachmentCount = 1,
					.pAttachments = &color_attachment,
					.subpassCount = 1,
					.pSubpasses = &subpass,
					.dependencyCount = 1,
					.pDependencies = &dependency,
				};

				auto result = vkCreateRenderPass(device, &create_info, nullptr, &renderpass);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"render pass\" for shader in pipeline.");
				}
			}
			_render_pass = Private::from_vulkan(renderpass);

			VkPipeline pipeline;
			{
				VkGraphicsPipelineCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.stageCount = static_cast<uint32_t>(stage_create_info.size()),
					.pStages = stage_create_info.data(),
					.pVertexInputState = &vertex_input_create_info,
					.pInputAssemblyState = &input_assembly_create_info,
					.pViewportState = &viewport_create_info,
					.pRasterizationState = &rasterization_create_info,
					.pMultisampleState = &multisample_create_info,
					.pDepthStencilState = nullptr,
					.pColorBlendState = &blend_create_info,
					.pDynamicState = &dynamic_create_info,
					.layout = pipeline_layout,
					.renderPass = renderpass,
					.subpass = 0,
					.basePipelineHandle = nullptr,
					.basePipelineIndex = -1,
				};

				auto result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline);

				switch (result)
				{
				case VK_SUCCESS: break;
				default: throw std::runtime_error("Could not set up \"render pass\" for pipeline.");
				}
			}
			_vulkan_pipeline = Private::from_vulkan(pipeline);

			_frame_buffers.reserve(frames.size());
			for (size_t frame_iterator = 0; frame_iterator < frames.size(); ++frame_iterator)
			{
				VkImageView swapchain_image = Private::to_vulkan(frames[frame_iterator].image);

				VkFramebufferCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass = renderpass,
					.attachmentCount = 1,
					.pAttachments = &swapchain_image,
					.width = width,
					.height = height,
					.layers = 1,
				};

				VkFramebuffer framebuffer;
				auto result = vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer);

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
			clear();
			throw;
		}
	}

	draw_pipeline::draw_pipeline(draw_pipeline&& other) noexcept
	{
		_settings = std::move(_settings);
		_layout = std::move(other._layout);
		_render_pass = std::move(other._render_pass);
		_vulkan_pipeline = std::move(other._vulkan_pipeline);

		other._layout = nullptr;
	}
	auto draw_pipeline::operator=(draw_pipeline&& other) noexcept -> draw_pipeline&
	{
		clear();

		_settings = std::move(_settings);
		_layout = std::move(other._layout);
		_render_pass = std::move(other._render_pass);
		_vulkan_pipeline = std::move(other._vulkan_pipeline);

		other._layout = nullptr;

		return *this;
	}

	void draw_pipeline::clear() noexcept
	{
		if (empty()) return;

		auto vulkan_device = Private::to_vulkan(target_window().device().vulkan_device());

		for (auto& framebuffer : _frame_buffers) vkDestroyFramebuffer(vulkan_device, Private::to_vulkan(framebuffer), nullptr);
		_frame_buffers.clear();
		if (_vulkan_pipeline) vkDestroyPipeline(vulkan_device, Private::to_vulkan(_vulkan_pipeline), nullptr);
		if (_render_pass) vkDestroyRenderPass(vulkan_device, Private::to_vulkan(_render_pass), nullptr);
		if (_layout) vkDestroyPipelineLayout(vulkan_device, Private::to_vulkan(_layout), nullptr);

		_layout = nullptr;
	}
}
