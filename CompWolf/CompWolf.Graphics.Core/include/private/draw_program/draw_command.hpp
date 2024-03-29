#ifndef COMPWOLF_GRAPHICS_DRAW_COMMAND_HEADER
#define COMPWOLF_GRAPHICS_DRAW_COMMAND_HEADER

#include "vulkan_types"
#include "gpu_program"
#include "draw_pipeline.hpp"
#include "draw_program.hpp"
#include "window"
#include <utility>
#include <freeable>
#include <vector>
#include <map>
#include <concepts>

namespace CompWolf::Graphics
{
	namespace Private
	{
		struct draw_data
		{
			gpu_index_buffer* indices;
			base_gpu_buffer* vertices;
			std::vector<base_gpu_buffer*> uniform_vertex_data;
		};

		class window_draw_command
		{
		private: // fields
			window_specific_pipeline* _pipeline;
			draw_data* _data;

		public: // getters
			auto window_pipeline() const noexcept -> const window_specific_pipeline& { return *_pipeline; }
			auto window_pipeline() noexcept -> window_specific_pipeline& { return *_pipeline; }

		public: // operators
			void operator()(const draw_program_input&);

		public: // constructors
			window_draw_command() = default;
			window_draw_command(window_draw_command&&) = default;
			auto operator=(window_draw_command&&) -> window_draw_command& = default;

			window_draw_command(window_specific_pipeline& pipeline, draw_data& data) : _pipeline(&pipeline), _data(&data)
			{}
		};
	}

	template <typename PipelineType>
	class draw_command
	{
		static_assert(std::same_as<PipelineType, PipelineType>, "draw_command was not given a proper pipeline");
	};

	template <typename VertexType, typename... UniformDataTypes>
	class draw_command<draw_pipeline<vertex_shader<VertexType, type_list<UniformDataTypes...>>>>
	{
	private: // fields
		using vertex_shader_type = vertex_shader<VertexType, type_list<UniformDataTypes...>>;
		using pipeline_type = draw_pipeline<vertex_shader_type>;
		pipeline_type* _pipeline;
		Private::draw_data _data;

		std::map<window*, Private::window_draw_command> _window_commands;

	public: // getters
		auto window_command(window& target) -> Private::window_draw_command&
		{
			auto iterator = _window_commands.find(&target);
			if (iterator != _window_commands.end()) return iterator->second;
			else
			{
				return _window_commands.try_emplace(&target, _pipeline->window_data(target), _data).first->second;
			}
		}

	public: // operators
		inline void operator()(const draw_program_input& args)
		{
			window_command(*args.target_window)(args);
		}

	public: // constructors
		draw_command() = default;
		draw_command(draw_command&&) = default;
		auto operator=(draw_command&&) -> draw_command& = default;

		draw_command(pipeline_type& pipeline
			, gpu_index_buffer& indices
			, gpu_vertex_buffer<VertexType>& vertices
			, gpu_uniform_buffer<UniformDataTypes>&... uniform_data
		) : _pipeline(&pipeline), _data{
			.indices = &indices,
			.vertices = &vertices,
			.uniform_vertex_data = std::vector<Private::base_gpu_buffer*>({&uniform_data...}),
		}
		{}
	};

	template <typename PipelineType, typename... BufferTypes>
	auto new_draw_command(PipelineType& pipeline
		, gpu_index_buffer& indices
		, BufferTypes&... buffers)
	{
		return draw_command<PipelineType>(pipeline, indices, buffers...);
	}
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_COMMAND_HEADER
