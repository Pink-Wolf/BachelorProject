#ifndef COMPWOLF_GRAPHICS_WINDOW_REBUILD_SURFACE_PARAMETERS_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_REBUILD_SURFACE_PARAMETERS_HEADER

namespace CompWolf::Graphics
{
	class window_surface;
	class window_swapchain;

	/* Aggregate type used by window's rebuild_surface-events. */
	struct window_rebuild_surface_parameters
	{
		/* For the event rebuilding_surface, the old surface;
		 * For the event rebuild_surface, the new surface.
		 */
		window_surface* surface;
		/* For the event rebuilding_swapchain, the old swapchain;
		 * For the event rebuild_swapchain, the new swapchain.
		 */
		window_swapchain* swapchain;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_REBUILD_SURFACE_PARAMETERS_HEADER
