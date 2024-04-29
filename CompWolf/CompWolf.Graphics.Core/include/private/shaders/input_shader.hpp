#ifndef COMPWOLF_GRAPHICS_INPUT_SHADER_HEADER
#define COMPWOLF_GRAPHICS_INPUT_SHADER_HEADER

#include "shader.hpp"

namespace CompWolf::Graphics
{
	/* A shader that can take a custom type of input.
	 * A program using this shader takes the given InputType as its input.
	 * 
	 * This is, in terms of the graphics pipeline, a "vertex" shader.
	 * 
	 * @typeparam InputType The type of data the shader takes as input. That is, FieldTypes specifies its vertex input, specified in glsl using layout(location = ?).
	 * The shader specifically takes multiple InputType-objects as its input
	 * 
	 * @typeparam FieldTypes The type of data the shader has, which is not unique to each input. That is, FieldTypes specifies its uniform buffers, specified in glsl using layout(binding = ?).
	 * Must be type_value_pairs, specifying its type and binding position.
	 */
	template <ShaderField InputType, typename... FieldTypes>
	class input_shader : public shader<FieldTypes...>
	{
		using shader<FieldTypes...>::shader;
	public: // type definitions
		/* The type that the shader takes as its input. */
		using input_type = InputType;
	};
}

#endif // ! COMPWOLF_GRAPHICS_INPUT_SHADER_HEADER
