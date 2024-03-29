#include "pch.h"
#include "gpu_program"

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace CompWolf::Graphics
{
	/******************************** getters ********************************/

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
			VkShaderModule shaderModule;
			{
				auto device = Private::to_vulkan(vulkan_device.vulkan_device());

				VkShaderModuleCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = _raw_code.size() * sizeof(spv_byte_type),
					.pCode = reinterpret_cast<const uint32_t*>(_raw_code.data()),
				};

				auto result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

				switch (result)
				{
				case VK_SUCCESS: break;
				case VK_ERROR_INVALID_SHADER_NV: throw std::invalid_argument("Could not compile given shader code; make sure it is valid SPIR-V code.");
				default: throw std::runtime_error("Could not create shader.");
				}
			}

			shader_module_pointer = Private::from_vulkan(shaderModule);
			_compiled_shader.insert({ &vulkan_device, shader_module_pointer });
		}

		return shader_module_pointer;
	}

	/******************************** CompWolf::freeable ********************************/

	void shader::free() noexcept
	{
		for (auto& [vulkan_device, shader_module] : _compiled_shader)
		{
			vkDestroyShaderModule(Private::to_vulkan(vulkan_device->vulkan_device()), Private::to_vulkan(shader_module), nullptr);
		}

		_environment = nullptr;
	}
}
