#ifndef COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER
#define COMPWOLF_GRAPHICS_PRIVATE_VULKAN_HEADER

#include "vulkan_types"
#include <functional>
#include <variant>
#include <version_number>
#include <type_traits>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace CompWolf::Graphics::Private
{
#define COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(library, library_type, compwolf_type)					\
	inline library_type to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }			\
	inline compwolf_type from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }		\

	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkInstance, vulkan_instance*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkDebugUtilsMessengerEXT, vulkan_debug_messenger*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkPhysicalDevice, vulkan_physical_device*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(vulkan, VkDevice, vulkan_device*)
	COMPWOLF_GRAPHICS_PRIVATE_DEFINE_CONVERTERS(glfw, GLFWwindow*, glfw_window*)

	inline uint32_t to_vulkan(version_number a) { return VK_MAKE_API_VERSION(0, a.major, a.minor, a.patch); }

	/* Invokes the given getter to get the size of its data, and then invokes it again to get the data.
	 * The getter must take a [input], [size_pointer], and [data_pointer], and return some [return_type], where [size_pointer] and [data_pointer] are pointers to where the getter should read/write the size/data.
	 * The getter should always output the size.
	 * If [data_pointer] is not null, the size must not be greater than the given size, and should output the data.
	 * @param handle_result Invoked immediately after invoking getter, passing along getter's return-value.
	 * @return The data received by the getter, in a vector.
	 * @typeparam InputType The type passed as input to the getter. [input] is of the type InputType.
	 * @typeparam SizeType [size_pointer] is of the type SizeType*.
	 * @typeparam T The type of data the getter returns an array of. [data_pointer] is of the type T*.
	 * @typeparam GetterReturnType The type returned by the getter, used to check if it failed.
	 */
	template <typename InputType, typename SizeType, typename T, typename GetterReturnType>
	auto get_size_and_vector(std::function<GetterReturnType(InputType, SizeType*, T*)> getter, InputType input, std::function<void(GetterReturnType)> handle_result) -> std::vector<T>
	{
		SizeType size;
		handle_result(getter(input, &size, nullptr));
		if (size <= 0) return std::vector<T>();

		std::vector<VkPhysicalDevice> data(size);
		handle_result(getter(input, &size, data.data()));
		data.resize(size);

		return data;
	}
	/* Invokes the given getter to get the size of its data, and then invokes it again to get the data.
	 * The getter must take a [input], [size_pointer], and [data_pointer], where [size_pointer] and [data_pointer] are pointers to where the getter should read/write the size/data.
	 * The getter should always output the size.
	 * If [data_pointer] is not null, the size must not be greater than the given size, and should output the data.
	 * @return The data received by the getter, in a vector.
	 * @typeparam InputType The type passed as input to the getter. [input] is of the type InputType.
	 * @typeparam SizeType [size_pointer] is of the type SizeType*.
	 * @typeparam T The type of data the getter returns an array of. [data_pointer] is of the type T*.
	 * @typeparam GetterReturnType The type returned by the getter.
	 */
	template <typename InputType, typename SizeType, typename T, typename GetterReturnType = void>
	auto get_size_and_vector(std::function<GetterReturnType(InputType, SizeType*, T*)> getter, InputType input) -> std::vector<T>
	{
		SizeType size;
		getter(input, &size, nullptr);
		if (size <= 0) return std::vector<T>();

		std::vector<T> data(size);
		getter(input, &size, data.data());
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
