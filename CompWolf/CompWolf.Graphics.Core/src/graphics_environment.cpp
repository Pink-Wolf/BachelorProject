#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <memory>
#include <format>
#include <iostream>

namespace CompWolf::Graphics
{
	/******************************** fields ********************************/

	bool graphics_environment::_constructed(false);

	/******************************** other methods ********************************/

	void graphics_environment::update()
	{
		if (!is_this_main_thread()) throw std::logic_error("graphics_environment.update() was called on a thread that is not the main graphics thread.");

		graphics_environment_update_parameter args;
		updating(args);
		glfwPollEvents();
		updated(args);
	}

	/******************************** constructors ********************************/

	void graphics_environment::setup()
	{
		if (_constructed) throw std::logic_error("Tried constructing a graphics_environment while one already exists.");
		_constructed = true;

		_main_graphics_thread = std::this_thread::get_id();

		_glfw_handle = glfw_handle(_settings);
		_vulkan_handle = vulkan_handle(_settings);
		_vulkan_debug_handle = vulkan_debug_handle(_settings, vulkan_instance());
		_gpus = gpu_manager(_settings, vulkan_instance());
	}
	graphics_environment::~graphics_environment()
	{
		_constructed = false;
	}

	template graphics_environment::graphics_environment(graphics_environment_settings);
	template graphics_environment::graphics_environment(const graphics_environment_settings&);
	template graphics_environment::graphics_environment(graphics_environment_settings&&);
}
