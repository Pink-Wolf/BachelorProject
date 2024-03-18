#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include "vulkan_debug_utils.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/******************************** constructors ********************************/

	vulkan_handle::vulkan_handle(const graphics_environment_settings& settings)
	{
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = settings.program_name.c_str(),
			.applicationVersion = Private::to_vulkan(settings.program_version),
			.pEngineName = "CompWolf",
			.engineVersion = Private::to_vulkan(compwolf_version),
			.apiVersion = VK_API_VERSION_1_3,
		};

		uint32_t glfw_extension_count;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (settings.internal_debug_callback)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		std::vector<const char*> validation_layers;
		if (settings.internal_debug_callback)
		{
			validation_layers = std::vector<const char*>{
				"VK_LAYER_KHRONOS_validation"
			};
		}

		auto debugMessengerCreateInfo = vulkan_debug_messenger_create_info(&settings.internal_debug_callback);

		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = settings.internal_debug_callback ? &debugMessengerCreateInfo : nullptr,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = static_cast<uint32_t>(validation_layers.size()),
			.ppEnabledLayerNames = validation_layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkInstance instance;
		auto result = vkCreateInstance(&createInfo, nullptr, &instance);

		switch (result)
		{
		case VK_SUCCESS: break;
		case VK_ERROR_LAYER_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine does not have the right layers.");
		case VK_ERROR_EXTENSION_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine does not have the right extensions.");
		case VK_ERROR_INCOMPATIBLE_DRIVER: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine has incompatible drivers.");
		default: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics.");
		}

		_vulkan_instance = Private::from_vulkan(instance);
	}

	/******************************** CompWolf::freeable ********************************/

	void vulkan_handle::free() noexcept
	{
		if (empty()) return;
		vkDestroyInstance(Private::to_vulkan(_vulkan_instance), nullptr);
		_vulkan_instance = nullptr;
	}
}
