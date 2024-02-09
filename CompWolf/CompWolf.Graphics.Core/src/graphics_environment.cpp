#include "pch.h"
#include "graphics_environment.h"

#include "private/compwolf_vulkan.h"
#include <stdexcept>
#include <memory>

namespace CompWolf::Graphics
{
	bool graphics_environment::constructed(false);

	graphics_environment::graphics_environment()
	{
		if (constructed) throw std::logic_error("Tried constructing a graphics_environment while one already exists.");

		glfw_handle = setup_glfw();
		vulkan_handle = setup_vulkan();
		main_graphics_thread = std::this_thread::get_id();

		constructed = true;
	}
	graphics_environment::~graphics_environment()
	{
		constructed = false;
	}
	auto graphics_environment::setup_glfw() -> glfw_handle_type
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

	auto graphics_environment::setup_vulkan() -> vulkan_handle_type
	{
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Compwolf App",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "CompWolf",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
		};

		uint32_t glfw_extension_count;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		VkInstanceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = 0,
			.enabledExtensionCount = glfw_extension_count,
			.ppEnabledExtensionNames = glfw_extensions,
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

	void graphics_environment::update()
	{
		graphics_environment_update_parameter args;
		updating(args);
		glfwPollEvents();
		updated(args);
	}
}
