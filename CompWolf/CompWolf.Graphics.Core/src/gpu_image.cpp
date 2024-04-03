#include "pch.h"
#include "gpu_program"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include <compwolf_utility>

namespace CompWolf::Graphics
{
	/******************************** other methods ********************************/

	void gpu_image_buffer::bind_to_shader(gpu_memory::bind_handle bind_handle) const
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto vkImage = Private::to_vulkan(vulkan_image());
		auto& bind_data = *Private::to_private(bind_handle);

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

	/******************************** CompWolf::Graphics::gpu_image_allocator ********************************/

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(VkImage, VkImage, Private::gpu_image_allocator::data_handle)

	auto Private::gpu_image_allocator::from_image(Private::vulkan_image data) const noexcept -> data_handle
	{
		return from_VkImage(to_vulkan(data));
	}
	auto Private::gpu_image_allocator::to_image(data_handle data) const noexcept -> Private::vulkan_image
	{
		return from_vulkan(to_VkImage(data));
	}

	auto Private::gpu_image_allocator::alloc_data() const -> data_handle
	{
		auto logicDevice = Private::to_vulkan(target_device->vulkan_device());

		VkImage image;
		{
			VkImageCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.extent{
					.width = static_cast<uint32_t>(width),
					.height = static_cast<uint32_t>(height),
					.depth = 1,
				},
				.mipLevels = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.initialLayout = VK_IMAGE_LAYOUT_GENERAL, // !!! This should be VK_IMAGE_LAYOUT_UNDEFINED
			};

			auto result = vkCreateImage(logicDevice, &createInfo, nullptr, &image);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_OUT_OF_HOST_MEMORY: throw std::runtime_error("Could not create image on GPU; not enough CPU space.");
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw std::runtime_error("Could not create image on GPU; not enough GPU space.");
			default: throw std::runtime_error("Could not create image on GPU.");
			}
		}

		return from_VkImage(image);
	}

	void Private::gpu_image_allocator::bind_data(data_handle data, vulkan_memory memory) const
	{
		vkBindImageMemory(Private::to_vulkan(target_device->vulkan_device())
			, to_VkImage(data)
			, Private::to_vulkan(memory)
			, 0
		);
	}

	void Private::gpu_image_allocator::free_data(data_handle data) const noexcept
	{
		vkDestroyImage(Private::to_vulkan(target_device->vulkan_device())
			, to_VkImage(data)
			, nullptr
		);
	}

	void Private::gpu_image_allocator::private_info(data_handle data, private_info_handle out_pointer) const
	{
		auto logicDevice = Private::to_vulkan(target_device->vulkan_device());
		auto image = to_VkImage(data);
		auto& out = *to_private(out_pointer);

		out.size = static_cast<std::size_t>(width) * height;
		vkGetImageMemoryRequirements(logicDevice, image, &out.memoryRequirements);
	}

	/******************************** CompWolf::freeable ********************************/

	void gpu_image_buffer::free() noexcept
	{
		if (empty()) return;

		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		
		if (_sampler) vkDestroySampler(logicDevice, Private::to_vulkan(_sampler), nullptr);
		if (_image_view) vkDestroyImageView(logicDevice, Private::to_vulkan(_image_view), nullptr);

		memory().free();
	}
}
