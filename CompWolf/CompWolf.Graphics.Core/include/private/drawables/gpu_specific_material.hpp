#ifndef COMPWOLF_GRAPHICS_GPU_SPECIFIC_MATERIAL_HEADER
#define COMPWOLF_GRAPHICS_GPU_SPECIFIC_MATERIAL_HEADER

#include "vulkan_types"
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_connection;
}

namespace CompWolf::Graphics::Private
{
	struct draw_material_data;

	/* Contains a draw_material's data and logic specific to a gpu. */
	class gpu_specific_material : basic_freeable
	{
	private: // fields
		owned_ptr<gpu_connection*> _gpu;
		Private::vulkan_pipeline_layout_descriptor _layout_descriptor;
		Private::vulkan_pipeline_layout _layout;

	public: // accessors
		/* Returns the gpu this material is for. */
		inline auto& gpu() noexcept { return *_gpu; }
		/* Returns the gpu this material is for. */
		inline auto& gpu() const noexcept { return *_gpu; }

	public: // vulkan-related
		/* Returns the pipeline's vulkan_pipeline_layout, representing a VkPipelineLayout. */
		inline auto vulkan_pipeline_layout() const noexcept { return _layout; }
		/* Returns the pipeline's vulkan_pipeline_layout_descriptor, representing a VkDescriptorSetLayout. */
		inline auto vulkan_pipeline_layout_descriptor() const noexcept { return _layout_descriptor; }

	public: // constructors
		/* Constructs a freed gpu_specific_material, as in one that cannot be used to draw anything. */
		gpu_specific_material() = default;
		gpu_specific_material(gpu_specific_material&&) = default;
		auto operator=(gpu_specific_material&&) -> gpu_specific_material& = default;
		inline ~gpu_specific_material() noexcept { free(); }

		gpu_specific_material(draw_material_data&, gpu_connection&);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_gpu;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SPECIFIC_MATERIAL_HEADER
