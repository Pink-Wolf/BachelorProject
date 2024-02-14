#include "pch.h"
#include "graphics"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <memory>
#include <format>
#include <iostream>

namespace CompWolf::Graphics
{
	std::atomic_bool graphics_environment::_constructed(false);

	void graphics_environment::setup()
	{
		auto old_constructed = _constructed.exchange(true);
		if (old_constructed) throw std::logic_error("Tried constructing a graphics_environment while one already exists.");

		_main_graphics_thread = std::this_thread::get_id();

		_glfw_handle = glfw_handle(_settings);
		_vulkan_handle = vulkan_handle(_settings);
		_vulkan_debug_handle = vulkan_debug_handle(_settings, get_vulkan_instance());
		_gpus = gpu_manager(_settings, get_vulkan_instance());
	}
	graphics_environment::~graphics_environment()
	{
		_constructed.store(false);
	}

	void graphics_environment::update()
	{
		if (!is_this_main_thread()) throw std::logic_error("graphics_environment.update() was called on a thread that is not the main graphics thread.");

		graphics_environment_update_parameter args;
		updating(args);
		glfwPollEvents();
		updated(args);
	}

	template graphics_environment::graphics_environment(graphics_environment_settings);
	template graphics_environment::graphics_environment(const graphics_environment_settings&);
	template graphics_environment::graphics_environment(graphics_environment_settings&&);
}
