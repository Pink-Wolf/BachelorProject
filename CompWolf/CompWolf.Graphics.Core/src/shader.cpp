#include "pch.h"
#include "shader"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	void shader::setup()
	{
		// setup() exists in case it is needed in the future
	}

	auto shader::shader_module(const gpu& vulkan_device) const -> Private::vulkan_shader
	{
		auto compiled_shader_iterator = _compiled_shader.find(&vulkan_device);
		Private::vulkan_shader shader_module_pointer;
		if (compiled_shader_iterator != _compiled_shader.end())
		{
			shader_module_pointer = compiled_shader_iterator->second;
		}
		else
		{
			VkShaderModule shader_module;
			{
				auto device = Private::to_vulkan(vulkan_device.vulkan_device());

				VkShaderModuleCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = _raw_code.size() * sizeof(spv_byte_type),
					.pCode = reinterpret_cast<const uint32_t*>(_raw_code.data()),
				};

				auto result = vkCreateShaderModule(device, &create_info, nullptr, &shader_module);

				switch (result)
				{
				case VK_SUCCESS: break;
				case VK_ERROR_INVALID_SHADER_NV: throw std::invalid_argument("Could not compile given shader code; make sure it is valid SPIR-V code.");
				default: throw std::runtime_error("Could not create shader.");
				}
			}

			shader_module_pointer = Private::from_vulkan(shader_module);
			_compiled_shader.insert({ &vulkan_device, shader_module_pointer });
		}

		return shader_module_pointer;
	}

	shader::~shader()
	{
		for (auto& [vulkan_device, shader_module] : _compiled_shader)
		{
			vkDestroyShaderModule(Private::to_vulkan(vulkan_device->vulkan_device()), Private::to_vulkan(shader_module), nullptr);
		}
	}
}
