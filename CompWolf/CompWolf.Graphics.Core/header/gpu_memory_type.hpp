#ifndef COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER
#define COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER

#include "compwolf_vulkan.hpp"
#include "gpu_program"

namespace CompWolf::Graphics::Private
{
	struct gpu_memory_private_info
	{
		VkMemoryRequirements memoryRequirements;
		std::size_t size;
	};
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(private, gpu_memory_private_info*, gpu_memory_allocator::private_info_handle)
		
	struct gpu_memory_bind_data
	{
		VkCommandBuffer commandBuffer;
		VkDescriptorSet descriptorSet;
		uint32_t bindingIndex;
	};
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(private, gpu_memory_bind_data*, gpu_memory::bind_handle)
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MEMORY_TYPE_HEADER
