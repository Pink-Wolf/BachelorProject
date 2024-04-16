#ifndef COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
#define COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER

#include "vulkan_types"
#include "graphics"
#include "window"
#include "gpu_program"

#include <utility>
#include <vector>
#include <owned>
#include <freeable>
#include <map>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct draw_pipeline_data
		{
			const std::vector<Private::shader_field_info_handle>* input_types;
			const std::vector<std::size_t>* input_offsets;
			std::size_t input_stride;

			enum class uniform_data_type
			{
				buffer,
				image,
			};
			std::vector<uniform_data_type> vertex_uniform_types;
			std::vector<uniform_data_type> fragment_uniform_types;

			base_shader* vertex_shader;
			base_shader* fragment_shader;
		};

		class gpu_specific_pipeline : public basic_freeable
		{
		private: // fields
			owned_ptr<gpu_connection*> _device;
			Private::vulkan_pipeline_layout_descriptor _layout_descriptor;
			Private::vulkan_pipeline_layout _layout;

		public: // getters
			auto device() noexcept -> gpu_connection& { return *_device; }
			auto device() const noexcept -> const gpu_connection& { return *_device; }

			inline auto layout_descriptor() const noexcept { return _layout_descriptor; }
			inline auto layout() const noexcept { return _layout; }

		public: // constructor
			gpu_specific_pipeline() = default;
			gpu_specific_pipeline(gpu_specific_pipeline&&) = default;
			auto operator=(gpu_specific_pipeline&&) -> gpu_specific_pipeline& = default;
			inline ~gpu_specific_pipeline() noexcept { free(); }

			gpu_specific_pipeline(gpu_connection&, const draw_pipeline_data&);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_device;
			}
			void free() noexcept final;
		};
	}

	class window_specific_pipeline : public basic_freeable, public window_user
	{
	private: // fields
		const Private::draw_pipeline_data* _pipeline_data;
		const Private::gpu_specific_pipeline* _gpu_data;

		Private::vulkan_descriptor_pool _descriptor_pool;
		std::vector<Private::vulkan_descriptor_set> _descriptor_sets;
		
		Private::vulkan_pipeline _pipeline;
		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

	protected: // event handlers
		void on_rebuild_surface(
			const event<window_rebuild_surface_parameters>&,
			window_rebuild_surface_parameters&
		) final;

	public: // getters
		inline auto pipeline_data() const noexcept -> const Private::draw_pipeline_data& { return *_pipeline_data; }
		inline auto gpu_data() const noexcept -> const Private::gpu_specific_pipeline& { return *_gpu_data; }

		inline auto vulkan_descriptor_pool() const noexcept { return _descriptor_pool; }
		inline auto vulkan_descriptor_sets() const noexcept
			-> const std::vector<Private::vulkan_descriptor_set>&
		{
			return _descriptor_sets;
		}

		inline auto vulkan_pipeline() const noexcept { return _pipeline; }
		inline auto vulkan_frame_buffer(std::size_t index) const noexcept { return _frame_buffers[index]; }

	private: // constructors
		void setup();
	public:
		window_specific_pipeline() = default;
		window_specific_pipeline(window_specific_pipeline&&) = default;
		auto operator=(window_specific_pipeline&&)->window_specific_pipeline & = default;
		inline ~window_specific_pipeline() noexcept { free(); }

		inline window_specific_pipeline(window& target_window, const Private::draw_pipeline_data& pipeline_data, const Private::gpu_specific_pipeline& gpu_data)
			: window_user(target_window)
			, _pipeline_data(&pipeline_data)
			, _gpu_data(&gpu_data)
		{
			if (&target_window.device() != &gpu_data.device())
				throw std::invalid_argument("Tried creating window-specific pipeline logic with gpu-data for a gpu other than the window's");
			setup();
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !has_window();
		}
		void free() noexcept final;
		event<> freeing;
	};

	namespace Private
	{
		class base_draw_pipeline : public basic_freeable
		{
		private: // fields
			draw_pipeline_data _pipeline_data;

			mutable std::map<gpu_connection*, Private::gpu_specific_pipeline> _gpu_data;

			mutable std::map<window*, window_specific_pipeline> _window_data;

		public: // getters
			auto window_data(window&) const noexcept -> const window_specific_pipeline&;
			inline auto window_data(window& a) noexcept -> window_specific_pipeline&
			{
				return const_cast<window_specific_pipeline&>(const_cast<const base_draw_pipeline*>(this)->window_data(a));
			}

		public: // constructors
			base_draw_pipeline() = default;
			base_draw_pipeline(base_draw_pipeline&&) = default;
			auto operator =(base_draw_pipeline&&) -> base_draw_pipeline& = default;
			inline ~base_draw_pipeline() noexcept
			{
				free();
			}

			inline base_draw_pipeline(draw_pipeline_data&& data)
				: _pipeline_data(data) {}

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return _gpu_data.empty();
			}
			void free() noexcept final
			{
				if (empty()) return;

				_window_data.clear();
				_gpu_data.clear();
			}
		};
	}

	template <typename VertexShaderType, typename FragmentShaderType>
	class draw_pipeline
	{
		static_assert(std::same_as<VertexShaderType, VertexShaderType>, "draw_pipeline was not given a proper vertex and fragment shader");
	};

	template <typename VertexType, typename... UniformVertexTypes, typename... FragmentVertexTypes>
	class draw_pipeline<vertex_shader<VertexType, type_list<UniformVertexTypes...>>, shader<type_list<FragmentVertexTypes...>>>
		: public Private::base_draw_pipeline
	{
	public: // fields
		using vertex_shader_type = vertex_shader<VertexType, type_list<UniformVertexTypes...>>;
		using fragment_shader_type = shader<type_list<FragmentVertexTypes...>>;

	public: // constructors
		draw_pipeline() = default;
		draw_pipeline(draw_pipeline&&) = default;
		auto operator =(draw_pipeline&&) -> draw_pipeline& = default;
		inline ~draw_pipeline() noexcept
		{
			free();
		}

		draw_pipeline(vertex_shader_type& vertex_shader, fragment_shader_type& fragment_shader)
			: base_draw_pipeline(Private::draw_pipeline_data
			{
				.input_types = &shader_field_info_handles<VertexType, std::vector<Private::shader_field_info_handle>>(),
				.input_offsets = &shader_field_info_offsets<VertexType, std::vector<std::size_t>>(),
				.input_stride = sizeof(VertexType),
				.vertex_uniform_types = std::vector<Private::draw_pipeline_data::uniform_data_type>
					({
						std::same_as<UniformVertexTypes, shader_image>
						? Private::draw_pipeline_data::uniform_data_type::image
						: Private::draw_pipeline_data::uniform_data_type::buffer
					...}),
				.fragment_uniform_types = std::vector<Private::draw_pipeline_data::uniform_data_type>
					({
						std::same_as<FragmentVertexTypes, shader_image>
						? Private::draw_pipeline_data::uniform_data_type::image
						: Private::draw_pipeline_data::uniform_data_type::buffer
					...}),
				.vertex_shader = static_cast<Private::base_shader*>(&vertex_shader),
				.fragment_shader = static_cast<Private::base_shader*>(&fragment_shader),
			}
		)
		{}
	};

	template <typename VertexShaderType, typename FragmentShaderType>
	auto new_draw_pipeline(VertexShaderType& vertex_shader, FragmentShaderType& fragment_shader)
	{
		return draw_pipeline<VertexShaderType, FragmentShaderType>(vertex_shader, fragment_shader);
	}
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
