#ifndef  COMPWOLF_GRAPHICS_CORE_GRAPHICS_ENVIRONMENT_HEADER
#define COMPWOLF_GRAPHICS_CORE_GRAPHICS_ENVIRONMENT_HEADER

#include "glfw_types.h"

namespace CompWolf::Graphics::Core
{
	class graphics_environment
	{
	public:
		/* Sets up application-wide logic used to handle graphics and windows.
		 * @throws std::runtime_error
		 */
		graphics_environment();
		~graphics_environment();

	private:
		static size_t environment_counter;
	};
}

#endif // ! COMPWOLF_GRAPHICS_CORE_GRAPHICS_ENVIRONMENT_HEADER
