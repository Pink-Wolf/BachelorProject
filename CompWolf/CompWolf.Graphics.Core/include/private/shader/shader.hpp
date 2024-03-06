#ifndef COMPWOLF_GRAPHICS_SHADER_HEADER
#define COMPWOLF_GRAPHICS_SHADER_HEADER

#include "vulkan_types"
#include "graphics"
#include <vector>
#include <map>
#include <cstdint>
#include <concepts>
#include <utility>

namespace CompWolf::Graphics
{
	enum class shader_type
	{
		vertex,
		frag,
	};

	class shader : public basic_environment_user
	{
	public:
		using spv_byte_type = char;

	private:
		using code_type = std::vector<spv_byte_type>;
		code_type _raw_code;

		using compiled_shader_type = std::map<const gpu*, Private::vulkan_shader>;
		mutable compiled_shader_type _compiled_shader;

	private:
		void setup();
	public:
		shader() = default;

		template<typename T>
			requires std::convertible_to<T, code_type>
		shader(graphics_environment& environment, T&& code) : basic_environment_user(environment),
			_raw_code(std::forward<T>(code))
		{
			setup();
		}

		~shader();

	public:
		auto shader_module(const gpu&) const -> Private::vulkan_shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_HEADER
