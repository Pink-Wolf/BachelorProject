#include "pch.h"
#include "shaders"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include "gpus"
#include <fstream>

namespace CompWolf::Graphics
{
	auto shader_code_from_file(std::string path) -> std::vector<uint32_t>
	{
		// file is read in individual bytes to keep little-endian.

		std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
		if (!stream.is_open()) throw std::runtime_error("Could not open " + path + "; make sure to run compile.bat to create the file.");

		std::vector<uint32_t> data;
		data.resize(stream.tellg() / 4);

		stream.seekg(0);
		stream.read(static_cast<char*>(static_cast<void*>(data.data())), data.size() * 4);

		return data;
	}

	/******************************** getters ********************************/

	auto Private::base_shader::vulkan_shader(const gpu_connection& vulkan_device) const -> Private::vulkan_shader
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
				auto logicDevice = Private::to_vulkan(vulkan_device.vulkan_device());

				VkShaderModuleCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.codeSize = _raw_code.size() * sizeof(uint32_t),
					.pCode = _raw_code.data(),
				};

				auto result = vkCreateShaderModule(logicDevice, &createInfo, nullptr, &shaderModule);

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

	void Private::base_shader::free() noexcept
	{
		for (auto& [vulkan_device, vulkan_shader] : _compiled_shader)
		{
			vkDestroyShaderModule(Private::to_vulkan(vulkan_device->vulkan_device()), Private::to_vulkan(vulkan_shader), nullptr);
		}

		_environment = nullptr;
	}
}
