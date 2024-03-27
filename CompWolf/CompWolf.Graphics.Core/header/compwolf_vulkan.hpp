#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER

#include "vulkan_types"
#include <functional>
#include <variant>
#include <version_number>
#include <compwolf_type_traits>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace CompWolf::Graphics::Private
{
#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(library, library_type, compwolf_type)					\
	inline auto to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }					\
	inline auto from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }				\

#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_POINTER_CONVERTERS(library, library_type, compwolf_type)			\
	inline auto to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }					\
	inline auto to_##library(const compwolf_type a) { return reinterpret_cast<const library_type>(a); }		\
	inline auto from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }				\
	inline auto from_##library(const library_type a) { return reinterpret_cast<const compwolf_type>(a); }	\

#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(library, library_type, compwolf_type)		\
	inline auto to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a.value); }			\
	inline auto from_##library(library_type a) { return compwolf_type(reinterpret_cast<uint64_t>(a)); }		\

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkFence, vulkan_fence)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkSemaphore, vulkan_semaphore)

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkInstance, vulkan_instance)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkDebugUtilsMessengerEXT, vulkan_debug_messenger)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkPhysicalDevice, vulkan_physical_device)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkDevice, vulkan_device)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkQueue, vulkan_queue);

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(glfw, GLFWwindow*, glfw_window)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkSurfaceKHR, vulkan_surface)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkSwapchainKHR, vulkan_swapchain)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkImageView, vulkan_image_view)
		
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkBuffer, vulkan_buffer)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkDeviceMemory, vulkan_memory)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkShaderModule, vulkan_shader)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkPipelineLayout, vulkan_pipeline_layout)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkDescriptorSetLayout, vulkan_pipeline_layout_descriptor)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkDescriptorPool, vulkan_descriptor_pool)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkDescriptorSet, vulkan_descriptor_set)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkRenderPass, vulkan_render_pass)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkPipeline, vulkan_pipeline)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkFramebuffer, vulkan_frame_buffer)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_NONDISPATCH_CONVERTERS(vulkan, VkCommandPool, vulkan_command_pool)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkCommandBuffer, vulkan_command)

	inline uint32_t to_vulkan(version_number a) { return VK_MAKE_API_VERSION(0, a.major, a.minor, a.patch); }

	/* Invokes the given getter to get the size of its data, and then invokes it again to get the data.
	 * The getter must take a [size_pointer], and [data_pointer], where [size_pointer] and [data_pointer] are pointers to where the getter should write the size/data respectively.
	 * The getter should always output the size.
	 * If [data_pointer] is not null, the size must not be greater than the given size, and should output the data.
	 * @return The data received by the getter, in a vector.
	 * @typeparam SizeType [size_pointer] is of the type SizeType*.
	 * @typeparam T The type of data the getter returns an array of. [data_pointer] is of the type T*.
	 * @typeparam GetterType The type of the getter.
	 */
	template <typename SizeType, typename T, typename GetterType>
		requires std::invocable<GetterType, SizeType*, T*>
	auto get_size_and_vector(GetterType getter) -> std::vector<T>
	{
		SizeType size;
		getter(&size, nullptr);
		if (size <= 0) return std::vector<T>();

		std::vector<T> data(size);
		getter(&size, data.data());
		data.resize(size);

		return data;
	}

	template <typename FunctionType>
	FunctionType create_vulkan_function(VkInstance instance, const char* function_name)
	{
		auto function = vkGetInstanceProcAddr(instance, function_name);
		return (FunctionType)function;
	}
#define COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, function_name)\
	auto function_name = ::CompWolf::Graphics::Private::create_vulkan_function<PFN_##function_name>(instance, #function_name)
}

#endif // ! COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
