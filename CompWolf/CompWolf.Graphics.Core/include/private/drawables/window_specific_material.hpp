#ifndef COMPWOLF_GRAPHICS_WINDOW_SPECIFIC_MATERIAL_HEADER
#define COMPWOLF_GRAPHICS_WINDOW_SPECIFIC_MATERIAL_HEADER

#include "draw_material_settings.hpp"
#include "vulkan_types"
#include <freeable>
#include <owned>
#include "window"
#include <vector>
#include "gpu_specific_material.hpp"
#include <optional>
#include <event>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct draw_material_data;
	}
	class gpu_connection;

	/* Contains a draw_material's data and logic specific to a window. */
	class window_specific_material : public basic_freeable, public window_user
	{
	private: // fields
		Private::gpu_specific_material* _gpu_data;

		Private::vulkan_descriptor_pool _descriptor_pool;
		std::vector<Private::vulkan_descriptor_set> _descriptor_sets;

		Private::vulkan_pipeline _pipeline;
		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

		const std::vector<std::size_t>* _field_indices;

	public: // accessors
		/* Returns the gpu this material is for. */
		inline auto& gpu() noexcept { return _gpu_data->gpu(); }
		/* Returns the gpu this material is for. */
		inline auto& gpu() const noexcept { return _gpu_data->gpu(); }

		/* Returns the binding position of the material's shaders' fields.
		 * The order matches the draw_material's field_types.
		 */
		inline auto& field_indices() const noexcept { return *_field_indices; }

	public: // vulkan-related
		/* Returns the pipeline's vulkan_pipeline_layout, representing a VkPipelineLayout. */
		inline auto vulkan_pipeline_layout() const noexcept { return _gpu_data->vulkan_pipeline_layout(); }
		/* Returns the pipeline's vulkan_pipeline_layout_descriptor, representing a VkDescriptorSetLayout. */
		inline auto vulkan_pipeline_layout_descriptor() const noexcept { return _gpu_data->vulkan_pipeline_layout_descriptor(); }

		/* Returns the pipeline's vulkan_descriptor_pool, representing a VkDescriptorPool. */
		inline auto vulkan_descriptor_pool() const noexcept { return _descriptor_pool; }
		/* Returns the pipeline's vulkan_descriptor_sets, representing some VkDescriptorSets. */
		inline auto& vulkan_descriptor_sets() const noexcept { return _descriptor_sets; }

		/* Returns the pipeline's vulkan_pipeline, representing a VkPipeline. */
		inline auto vulkan_pipeline() const noexcept { return _pipeline; }
		/* Returns the pipeline's vulkan_frame_buffer for the frame with the given index in window_swapchain's frames(), representing a VkFramebuffer.
		 * @throws std::out_of_range if there is no frame at the given index.
		 */
		inline auto vulkan_frame_buffer(std::size_t i) const { return _frame_buffers.at(i); }
		
	public: // constructors
		/* Constructs a freed window_specific_material, as in one that cannot be used to draw anything. */
		window_specific_material() = default;
		window_specific_material(window_specific_material&&) = default;
		auto operator=(window_specific_material&&) -> window_specific_material& = default;
		inline ~window_specific_material() noexcept { free(); }

		window_specific_material(Private::draw_material_data&, Private::gpu_specific_material&, window&);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !has_window();
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SPECIFIC_MATERIAL_HEADER
