#ifndef COMPWOLF_GRAPHICS_SHADER_HEADER
#define COMPWOLF_GRAPHICS_SHADER_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>
#include "shader_field_info.hpp"
#include <vector>
#include <set>
#include <map>
#include <compwolf_type_traits>
#include <string>
#include <event>

namespace CompWolf
{
	class graphics_environment;
	class gpu_connection;

	/* Gets the SPIR-V code from the given file. SPIR-V code is used to construct a shader.
	 * @throws std::runtime_error if the given file could not be found or opened.
	 */
	auto shader_code_from_file(std::string) -> std::vector<uint32_t>;

	namespace Private
	{
		struct gpu_specific_shader_data
		{
			Private::vulkan_shader vulkan_shader;
			event<>::key_type gpu_freeing_key;
		};

		class base_shader : public basic_freeable
		{
		private: // fields
			std::vector<uint32_t> _raw_code;

			using compiled_shader_type = std::map<gpu_connection*, gpu_specific_shader_data>;
			mutable compiled_shader_type _compiled_shader;

		public: // getters
			/* Returns the shader's vulkan_shader, representing a VkShaderModule, for the given gpu.
			 * @throws std::runtime_error if there was an error getting the vulkan_shader due to causes outside of the program.
			 */
			auto vulkan_shader(gpu_connection&) const -> Private::vulkan_shader;

		public: // constructors
			/* Constructs a freed shader, that is one that cannot be run. */
			base_shader() = default;
			base_shader(base_shader&&) = default;
			auto operator=(base_shader&&) -> base_shader& = default;
			inline ~base_shader() noexcept { free(); }

			/* Constructs a shader with the given SPIR-V code.
			 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
			 */
			base_shader(std::vector<uint32_t>&& code)
				: _raw_code(std::move(code))
			{}

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return _compiled_shader.empty();
			}
			void free() noexcept final;
		};
	}

	/* Some code that can be run on a gpu.
	 * A shader receives some inputs, and for each input runs its code and outputs some values.
	 * @typeparam The type and binding position of data the shader has, which is not unique to each input. That is, FieldTypes specifies its uniform buffers, specified in glsl using layout(binding = ?).
	 * Must be type_value_pairs, specifying its type and binding position.
	 */
	template <typename... FieldTypes>
	class shader : public Private::base_shader
	{
		static_assert(dependent_false<FieldTypes...>,
			"shader was not given proper type_value_pairs determining data type and binding index."
			);
	};
	template <typename... FieldTypes, std::size_t... FieldIndices>
	class shader<type_value_pair<FieldTypes, FieldIndices>...> : public Private::base_shader
	{
		using Private::base_shader::base_shader;
	public: // type definitions
		/* A collection of type_value_pairs, denoting the shader's fields' type and binding position.
		 * Sorted by binding position.
		 */
		using field_types = type_list<type_value_pair<FieldTypes, FieldIndices>...>;

	public: // fields
		/* The indices given by FieldTypes, in a set. */
		static inline std::set<std::size_t> field_indices = std::set<std::size_t>({ FieldIndices ... });
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER_HEADER
