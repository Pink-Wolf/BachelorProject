#ifndef COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME_HEADER
#define COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME_HEADER

#include "vulkan_types"
#include "gpu_programs"

namespace CompWolf
{
	/* Aggregate type containing data about an actual image that is being drawn before being displayed on a window. */
	struct swapchain_frame
	{
		/* Programs drawing to the image should normally be using this pool to keep them synchronized between each other. */
		gpu_program_manager draw_job;

		/* A vulkan_image_view, representing a VkImageView pointing towards the actual image. */
		Private::vulkan_image_view image;
		/* The image's vulkan_frame_buffer, representing a VkFramebuffer. */
		Private::vulkan_frame_buffer frame_buffer;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME_HEADER
