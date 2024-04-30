#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include "vulkan_debug_utils.hpp"
#include <stdexcept>

namespace CompWolf
{
	/******************************** constructors ********************************/

	vulkan_debug_handle::vulkan_debug_handle(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance)
		: _teardowner(nullptr)
	{
		if (!settings.internal_debug_callback) return;

		auto instance = Private::to_vulkan(vulkan_instance);

		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
		if (vkCreateDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf debugger; could not create constructor for Vulkan debug messenger.");
		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
		if (vkDestroyDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf debugger; could not create destructor for Vulkan debug messenger.");

		auto createInfo = vulkan_debug_messenger_create_info(&settings.internal_debug_callback);

		VkDebugUtilsMessengerEXT debugMessenger;
		auto result = vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not set up CompWolf debugger; could not create Vulkan debug messenger.");
		}

		_messenger = Private::from_vulkan(debugMessenger);
		_teardowner = [vkDestroyDebugUtilsMessengerEXT, instance](Private::vulkan_debug_messenger vulkan_messenger)
			{
				auto messenger = Private::to_vulkan(vulkan_messenger);
				vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
			};
	}
}
