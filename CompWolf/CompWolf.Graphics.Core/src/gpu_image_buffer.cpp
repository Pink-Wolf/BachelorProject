#include "pch.h"
#include "gpu_buffers"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include "shaders"
#include "gpus"
#include <compwolf_utility>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	void gpu_image_buffer::setup()
	{
		_image_view = nullptr;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(Private::to_vulkan(device().vulkan_physical_device()), &physicalDeviceProperties);

		VkImageView imageView;
		{
			VkImageViewCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Private::to_vulkan(vulkan_image()),
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.subresourceRange{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};

			auto result = vkCreateImageView(logicDevice, &createInfo, nullptr, &imageView);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not get connection to an image");
			}

			_image_view = Private::from_vulkan(imageView);
		}

		VkSampler vkSampler;
		{
			VkSamplerCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter = VK_FILTER_LINEAR,
				.minFilter = VK_FILTER_LINEAR,
				.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
				.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				.mipLodBias = .0f,
				.anisotropyEnable = VK_TRUE,
				.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy,
				.compareEnable = VK_FALSE,
				.minLod = .0f,
				.maxLod = .0f,
				.unnormalizedCoordinates = VK_FALSE,
			};

			auto result = vkCreateSampler(logicDevice, &createInfo, nullptr, &vkSampler);

			switch (result)
			{
			case VK_SUCCESS: break;
			default: throw std::runtime_error("Could not create \"sampler\" used by shader to access an image");
			}

			_sampler = Private::from_vulkan(vkSampler);
		}
	}

	/******************************** CompWolf::base_gpu_buffer ********************************/

	void gpu_image_buffer::bind_to_shader(Private::gpu_memory_bind_data* bind_ptr) const
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto vkImage = Private::to_vulkan(vulkan_image());
		auto& bind_data = *bind_ptr;

		VkDescriptorImageInfo imageInfo{
			.sampler = Private::to_vulkan(vulkan_sampler()),
			.imageView = Private::to_vulkan(vulkan_image_view()),
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
		};
		VkWriteDescriptorSet writer{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = bind_data.descriptorSet,
			.dstBinding = bind_data.bindingIndex,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &imageInfo,
		};

		vkUpdateDescriptorSets(logicDevice, 1, &writer, 0, nullptr);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_image_buffer::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		if (_sampler) vkDestroySampler(logicDevice, Private::to_vulkan(_sampler), nullptr);
		if (_image_view) vkDestroyImageView(logicDevice, Private::to_vulkan(_image_view), nullptr);

		base_gpu_buffer::free();
	}
}
