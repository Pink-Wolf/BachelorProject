#ifndef COMPWOLF_GRAPHICS_STATIC_SHADER_HEADER
#define COMPWOLF_GRAPHICS_STATIC_SHADER_HEADER

#include "shader.hpp"
#include <freeable>
#include <concepts>
#include <utility>

namespace CompWolf
{
	/* A shader whose code is from a file at the given path.
	 * Multiple instances of this class share resources, meaning you can construct instances of this whenever you need it without worrying about memory.
	 *
	 * @typeparam Path A pointer to a null-terminated string, which denotes the path to the .spv-file containing the code for the shader.
	 *
	 * @typeparam ShaderType The type of shader, like a template instance of shader or input_shader.
	 * Must be constructible with the same parameters as shader.
	 */
	template <const char* const Path, typename ShaderType>
		requires std::constructible_from<ShaderType, std::vector<uint32_t>&&>
	class static_shader : public basic_freeable
	{
	public: // type definitions
		/* The type of shader this contains. */
		using shader_type = ShaderType;

	private: // fields
		static inline shader_type _shader = shader_type();
		static inline std::size_t _instance_count = 0;
		bool _not_empty = false;

	public: // accessors
		/* Returns the shader this contains. */
		inline auto& shader() noexcept { return _shader; }
		/* Returns the shader this contains. */
		inline auto& shader() const noexcept { return _shader; }

	public: // constructors
		/* Constructs a freed shader, that is one that cannot be run. */
		static_shader() = default;
		inline static_shader(const static_shader&) noexcept
		{
			++_instance_count;
			_not_empty = true;
		}
		inline auto operator=(const static_shader&) noexcept -> static_shader&
		{
			++_instance_count;
			_not_empty = true;
		}
		inline static_shader(static_shader&& other) noexcept
		{
			_not_empty = true;
			other._not_empty = false;
		}
		inline auto operator=(static_shader&& other) noexcept -> static_shader&
		{
			_not_empty = true;
			other._not_empty = false;
		}
		inline ~static_shader() noexcept { free(); }

		/* If the bool is true, constructs the shader; if false, constructs a freed shader, that is one that cannot be run.
		 * @throws std::runtime_error if the file with the shader's code could not be found or opened.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 */
		inline static_shader(bool b) : _not_empty(b)
		{
			if (!b) return;

			++_instance_count;
			if (_shader.empty()) _shader = shader_type(shader_code_from_file(Path));
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_not_empty;
		}
		void free() noexcept final
		{
			if (empty()) return;

			--_instance_count;
			if (_instance_count == 0) _shader.free();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_STATIC_SHADER_HEADER
