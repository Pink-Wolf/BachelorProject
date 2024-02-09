#include "pch.h"
#include "graphics_environment.h"

#include "private/compwolf_vulkan.h"
#include <stdexcept>
#include <memory>
#include <format>
#include <iostream>

namespace CompWolf::Graphics
{
	VkBool32 graphics_environment_debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data
	) {
		const char* severity_string;
		switch (message_severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: severity_string = "verbose"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: severity_string = "info"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity_string = "warning"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: severity_string = "error"; break;
		default: severity_string = "unknown_severity"; break;
		}

		const char* type_string;
		switch (message_type)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: type_string = "general"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: type_string = "validation"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: type_string = "performance"; break;
		default: type_string = "unknown_type"; break;
		}

		auto message = std::format("CompWolf::Graphics debugger: {} {}:\n{}\n", type_string, severity_string, callback_data->pMessage);

		auto& environment = *static_cast<const graphics_environment*>(user_data);
		environment.report_debug_message(message);

		return VK_FALSE; // Debug callback should always return VK_FALSE
	};
	void graphics_environment::report_debug_message(std::string message) const
	{
		if (settings.internal_debug_callback) settings.internal_debug_callback(message);
	}
	auto get_vulkan_debug_messenger_create_info(const graphics_environment& environment) -> VkDebugUtilsMessengerCreateInfoEXT
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = graphics_environment_debug_callback,
		.pUserData = const_cast<graphics_environment*>(&environment), // As long as graphics_environment_debug_callback treats it as const, then this is fine.
		};

		return create_info;
	}

	bool graphics_environment::constructed(false);

	void graphics_environment::setup()
	{
		if (constructed) throw std::logic_error("Tried constructing a graphics_environment while one already exists.");

		glfw_handle = setup_glfw();
		vulkan_handle = setup_vulkan();
		vulkan_debug_handle = setup_debugger();
		main_graphics_thread = std::this_thread::get_id();

		constructed = true;
	}
	graphics_environment::~graphics_environment()
	{
		constructed = false;
	}
	auto graphics_environment::setup_glfw() const -> glfw_handle_type
	{
		auto result = glfwInit();

		if (result != GLFW_TRUE)
		{
			throw std::runtime_error("Could not set up glfw, used by CompWolf::Graphics");
		}

		return glfw_handle_type(empty_pointer(), teardown_glfw());
	}
	void graphics_environment::teardown_glfw::operator()(empty_pointer) const
	{
		glfwTerminate();
	}

	auto graphics_environment::setup_vulkan() const -> vulkan_handle_type
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

		auto debug_messenger_create_info = get_vulkan_debug_messenger_create_info(*this);

		VkInstanceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = settings.internal_debug_callback ? &debug_messenger_create_info : nullptr,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = static_cast<uint32_t>(validation_layers.size()),
			.ppEnabledLayerNames = validation_layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkInstance instance;
		auto result = vkCreateInstance(&create_info, nullptr, &instance);

		switch (result)
		{
		case VK_SUCCESS: break;
		case VK_ERROR_LAYER_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine does not have the right layers.");
		case VK_ERROR_EXTENSION_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine does not have the right extensions.");
		case VK_ERROR_INCOMPATIBLE_DRIVER: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics; the machine has incompatible drivers.");
		default: throw std::runtime_error("Could not set up vulkan, used by CompWolf::Graphics.");
		}

		return vulkan_handle_type(Private::from_vulkan(instance), teardown_vulkan());
	}
	void graphics_environment::teardown_vulkan::operator()(Private::vulkan_instance* vulkan_instance) const
	{
		auto instance = Private::to_vulkan(vulkan_instance);

		vkDestroyInstance(instance, nullptr);
	}

	auto graphics_environment::setup_debugger() const -> vulkan_debug_handle_type
	{
		if (!settings.internal_debug_callback)
		{
			return vulkan_debug_handle_type(nullptr);
		}

		auto instance = Private::to_vulkan(get_vulkan_instance());

		auto test = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
		if (vkCreateDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create constructor for Vulkan debug messenger.");
		COMPWOLF_GRAPHICS_DECLARE_DEFINE_VULKAN_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
		if (vkDestroyDebugUtilsMessengerEXT == nullptr) throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create destructor for Vulkan debug messenger.");

		auto create_info = get_vulkan_debug_messenger_create_info(*this);

		VkDebugUtilsMessengerEXT debug_messenger;
		auto result = vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debug_messenger);
		
		switch (result)
		{
		case VK_SUCCESS: break;
		default: throw std::runtime_error("Could not set up CompWolf::Graphics debugger; could not create Vulkan debug messenger.");
		}

		teardown_vulkan_debug teardowner{
			.teardowner = [vkDestroyDebugUtilsMessengerEXT, instance](Private::vulkan_debug_messenger* vulkan_messenger) {
				auto messenger = Private::to_vulkan(vulkan_messenger);
				vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
			}
		};

		return vulkan_debug_handle_type(Private::from_vulkan(debug_messenger), teardowner);
	}
	void graphics_environment::teardown_vulkan_debug::operator()(Private::vulkan_debug_messenger* vulkan_debugger) const
	{
		teardowner(vulkan_debugger);
	}

	void graphics_environment::update()
	{
		graphics_environment_update_parameter args;
		updating(args);
		glfwPollEvents();
		updated(args);
	}

	template graphics_environment::graphics_environment(graphics_environment_settings);
	template graphics_environment::graphics_environment(const graphics_environment_settings&);
	template graphics_environment::graphics_environment(graphics_environment_settings&&);
}
