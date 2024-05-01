#include "pch.h"
#include "shaders"

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include "gpus"
#include <fstream>

namespace CompWolf
{
	auto shader_code_from_file(std::string path) -> std::vector<uint32_t>
	{
		// file is read in individual bytes to keep little-endian.

		std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
		if (!stream.is_open()) throw std::runtime_error("Could not open \"" + path + "\".");

		std::vector<uint32_t> data;
		data.resize(stream.tellg() / 4);

		stream.seekg(0);
		stream.read(static_cast<char*>(static_cast<void*>(data.data())), data.size() * 4);

		return data;
	}

	/******************************** getters ********************************/

	auto Private::base_shader::vulkan_shader(gpu_connection& gpu) const -> Private::vulkan_shader
	{
		auto compiled_shader_iterator = _compiled_shader.find(&gpu);
		Private::vulkan_shader shader_module_pointer;
		if (compiled_shader_iterator != _compiled_shader.end())
		{
			shader_module_pointer = compiled_shader_iterator->second.vulkan_shader;
		}
		else
		{
			VkShaderModule shaderModule;
			{
				auto logicDevice = Private::to_vulkan(gpu.vulkan_device());

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
			auto gpu_freeing_key = gpu.freeing.subscribe([this, &gpu]()
				{
					_compiled_shader.erase(&gpu);
				}
			);
			_compiled_shader.insert({ &gpu, { shader_module_pointer, gpu_freeing_key} });
		}

		return shader_module_pointer;
	}

	/******************************** CompWolf::freeable ********************************/

	void Private::base_shader::free() noexcept
	{
		if (empty()) return;

		for (auto& [gpu, data] : _compiled_shader)
		{
			vkDestroyShaderModule(Private::to_vulkan(gpu->vulkan_device()), Private::to_vulkan(data.vulkan_shader), nullptr);
			gpu->freeing.unsubscribe(data.gpu_freeing_key);
		}

		_compiled_shader.clear();
	}
}
