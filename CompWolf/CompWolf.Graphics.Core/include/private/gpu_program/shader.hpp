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
	using shader_code_char_type = char;
	using shader_code_type = std::vector<shader_code_char_type>;

	namespace Private
	{
		class base_shader : public basic_freeable
		{
		private: // fields
			owned_ptr<graphics_environment*> _environment;
			shader_code_type _raw_code;

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
			base_shader() = default;
			base_shader(base_shader&&) = default;
			auto operator =(base_shader&&) -> base_shader& = default;
			inline ~base_shader() noexcept
			{
				free();
			}

			template<typename T>
				requires std::constructible_from<shader_code_type, T>
			base_shader(graphics_environment& environment, T&& code) : _environment(&environment),
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

	template <typename UniformDataTypeList>
	class shader : public Private::base_shader
	{
		static_assert(std::same_as<UniformDataTypeList, UniformDataTypeList>,
			"shader was not given type_lists of uniform data types"
			);
	};
	template <typename... UniformDataTypes>
	class shader<type_list<UniformDataTypes...>> : public Private::base_shader
	{
		using Private::base_shader::base_shader;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_HEADER
