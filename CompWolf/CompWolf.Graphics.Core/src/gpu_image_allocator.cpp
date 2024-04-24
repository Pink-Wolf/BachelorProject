#include "pch.h"
#include "gpu_buffers"

#include "compwolf_vulkan.hpp"
#include "gpu_memory_type.hpp"
#include <compwolf_utility>
#include "gpus"

namespace CompWolf::Graphics
{
	/******************************** CompWolf::base_gpu_buffer_allocator ********************************/

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(VkImage, VkImage, base_gpu_buffer_allocator::data_handle)

	auto gpu_image_allocator::from_image(Private::vulkan_image data) const noexcept -> data_handle
	{
		return from_VkImage(Private::to_vulkan(data));
	}
	auto gpu_image_allocator::to_image(data_handle data) const noexcept -> Private::vulkan_image
	{
		return Private::from_vulkan(to_VkImage(data));
	}

	auto gpu_image_allocator::alloc_data() const -> data_handle
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());

		VkImage image;
		{
			VkImageCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.extent{
					.width = static_cast<uint32_t>(width()),
					.height = static_cast<uint32_t>(height()),
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
	auto gpu_image_allocator::alloc_cpu_access(data_handle a) const -> data_handle
	{
		// !!! This is unused
		return a;
	}

	void gpu_image_allocator::bind_data(data_handle data, Private::vulkan_memory memory) const
	{
		vkBindImageMemory(Private::to_vulkan(device().vulkan_device())
			, to_VkImage(data)
			, Private::to_vulkan(memory)
			, 0
		);
	}

	void gpu_image_allocator::free_cpu_access(data_handle data) const noexcept {}
	void gpu_image_allocator::free_data(data_handle data) const noexcept
	{
		vkDestroyImage(Private::to_vulkan(device().vulkan_device())
			, to_VkImage(data)
			, nullptr
		);
	}

	void gpu_image_allocator::private_info(data_handle data, Private::gpu_buffer_private_info* out_pointer) const
	{
		auto logicDevice = Private::to_vulkan(device().vulkan_device());
		auto image = to_VkImage(data);
		auto& out = *out_pointer;

		out.size = static_cast<std::size_t>(width()) * height();
		vkGetImageMemoryRequirements(logicDevice, image, &out.memoryRequirements);
	}
}
