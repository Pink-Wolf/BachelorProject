#ifndef COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER
#define COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER

#include "compwolf_vulkan.hpp"
#include "gpu_program"

namespace CompWolf::Graphics::Private
{
	struct gpu_buffer_private_info
	{
		VkMemoryRequirements memoryRequirements;
		std::size_t size;
	};
		
	struct gpu_memory_bind_data
	{
		VkCommandBuffer commandBuffer;
		VkDescriptorSet descriptorSet;
		uint32_t bindingIndex;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER
