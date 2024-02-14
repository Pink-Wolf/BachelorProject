#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include "vulkan_debug_utils.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	vulkan_debug_handle::vulkan_debug_handle(const graphics_environment_settings& settings, Private::vulkan_instance vulkan_instance)
	{
		if (!settings.internal_debug_callback)
		{
			_messenger = nullptr;
			_teardowner = nullptr;
			return;
		}

		auto instance = Private::to_vulkan(vulkan_instance);

		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
		if (vkCreateDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create constructor for Vulkan debug messenger.");
		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
		if (vkDestroyDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create destructor for Vulkan debug messenger.");

		auto create_info = vulkan_debug_messenger_create_info(&settings.internal_debug_callback);

		VkDebugUtilsMessengerEXT debug_messenger;
		auto result = vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debug_messenger);

		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create Vulkan debug messenger.");
		}

		_messenger = Private::from_vulkan(debug_messenger);
		_teardowner = [vkDestroyDebugUtilsMessengerEXT, instance](Private::vulkan_debug_messenger vulkan_messenger)
			{
				auto messenger = Private::to_vulkan(vulkan_messenger);
				vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
			};
	}
	vulkan_debug_handle::~vulkan_debug_handle()
	{
		if (_teardowner) _teardowner(_messenger);
	}

	vulkan_debug_handle::vulkan_debug_handle(vulkan_debug_handle&& other) noexcept
	{
		_messenger = other._messenger;
		_teardowner = other._teardowner;
		other._teardowner = nullptr;
	}
	vulkan_debug_handle& vulkan_debug_handle::operator=(vulkan_debug_handle&& other) noexcept
	{
		if (_teardowner) _teardowner(_messenger);

		_messenger = other._messenger;
		_teardowner = other._teardowner;
		other._teardowner = nullptr;

		return *this;
	}
}
