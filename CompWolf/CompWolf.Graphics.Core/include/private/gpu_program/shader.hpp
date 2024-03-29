#ifndef COMPWOLF_GRAPHICS_SHADER_HEADER
#define COMPWOLF_GRAPHICS_SHADER_HEADER

#include "vulkan_types"
#include "graphics"
#include <vector>
#include <map>
#include <cstdint>
#include <concepts>
#include <utility>
#include <owned>
#include <freeable>

namespace CompWolf::Graphics
{
	class shader : public basic_freeable
	{
	public: // fields
		using spv_byte_type = char;
	private:
		owned_ptr<graphics_environment*> _environment;

		using code_type = std::vector<spv_byte_type>;
		code_type _raw_code;

		using compiled_shader_type = std::map<const gpu*, Private::vulkan_shader>;
		mutable compiled_shader_type _compiled_shader;

	public: // getters
		inline auto environment() noexcept -> graphics_environment&
		{
			return *_environment;
		}
		inline auto environment() const noexcept -> const graphics_environment&
		{
			return *_environment;
		}

		auto shader_module(const gpu&) const -> Private::vulkan_shader;

	public: // constructors
		shader() = default;
		shader(shader&&) = default;
		auto operator =(shader&&) -> shader& = default;
		inline ~shader() noexcept
		{
			free();
		}

		template<typename T>
			requires std::convertible_to<T, code_type>
		shader(graphics_environment& environment, T&& code) : _environment(&environment),
			_raw_code(std::forward<T>(code))
		{

		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_environment;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_HEADER
