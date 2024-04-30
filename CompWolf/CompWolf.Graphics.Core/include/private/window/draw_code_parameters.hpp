#ifndef COMPWOLF_GRAPHICS_DRAW_CODE_PARAMETERS_HEADER
#define COMPWOLF_GRAPHICS_DRAW_CODE_PARAMETERS_HEADER

#include "gpu_programs"
#include <utility>

namespace CompWolf
{
	class window;
	struct swapchain_frame;

	/* Aggregate type containing data passed by gpu_program, which are created by window::new_draw_program, to its code. */
	struct draw_code_parameters : public gpu_program_code_parameters
	{
		/* The window to draw onto. */
		const window* target_window;
		/* The window's frame to draw onto. */
		const swapchain_frame* frame;
		/* The index of frame in window_swapchain.frames(). */
		std::size_t frame_index;
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_CODE_PARAMETERS_HEADER
