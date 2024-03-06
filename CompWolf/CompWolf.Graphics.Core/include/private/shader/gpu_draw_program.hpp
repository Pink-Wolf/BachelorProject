#ifndef COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER

#include "vulkan_types"
#include "graphics"
#include "gpu_command.hpp"
#include "draw_pipeline.hpp"
#include <concepts>
#include <tuple>
#include <utility>
#include <vector>
#include <optional>

namespace CompWolf::Graphics
{
	namespace Private
	{
		class gpu_draw_program_frame_data : public window_user
		{
		private:
			draw_pipeline* _pipeline = nullptr;
			size_t _index;
			Private::vulkan_command _vulkan_command;

		public:
			gpu_draw_program_frame_data() = default;
			gpu_draw_program_frame_data(const gpu_draw_program_frame_data&) noexcept = delete;
			auto operator =(const gpu_draw_program_frame_data&) noexcept -> gpu_draw_program_frame_data & = delete;
			gpu_draw_program_frame_data(gpu_draw_program_frame_data&& other) noexcept;
			auto operator =(gpu_draw_program_frame_data&&) noexcept -> gpu_draw_program_frame_data&;

			gpu_draw_program_frame_data(draw_pipeline& pipeline, gpu_command& command, size_t index);

			void clear() noexcept;
			inline ~gpu_draw_program_frame_data()
			{
				clear();
			}

		public:
			inline auto empty() const noexcept -> bool
			{
				return !_pipeline;
			}

			inline auto vulkan_command() const
			{
				return _vulkan_command;
			}
			inline auto vulkan_command_pool() const
			{
				return _pipeline->target_window().swapchain().frames()[_index].command_pool;
			}

			inline auto target_window() -> window& final
			{
				return _pipeline->target_window();
			}
			inline auto target_window() const -> const window& final
			{
				return _pipeline->target_window();
			}

			inline auto frame() -> swapchain_frame&
			{
				return target_window().swapchain().frames()[_index];
			}

		public:
			void draw();

			using frameset_type = std::vector<gpu_draw_program_frame_data>;
			static auto new_frameset(draw_pipeline& pipeline, gpu_command& command) -> frameset_type;
			static auto next_frame(frameset_type&) -> gpu_draw_program_frame_data&;
		};
	}

	template <typename CommandType>
		requires std::is_base_of_v<gpu_command, CommandType>
	class gpu_draw_program : public window_user
	{
	public:
		using command_type = CommandType;
	private:
		using frame_data = Private::gpu_draw_program_frame_data;
	private:
		draw_pipeline* _pipeline;
		command_type _command;

		frame_data::frameset_type _programs_for_frames;

	public:
		inline auto target_window() -> window & final
		{
			return pipeline().target_window();
		}
		inline auto target_window() const -> const window & final
		{
			return pipeline().target_window();
		}

		inline auto pipeline() noexcept -> draw_pipeline&
		{
			return *_pipeline;
		}
		inline auto pipeline() const noexcept -> const draw_pipeline&
		{
			return *_pipeline;
		}

		inline auto command() noexcept -> gpu_command&
		{
			return _command;
		}
		inline auto command() const noexcept -> const gpu_command&
		{
			return _command;
		}

	public:
		inline auto empty() const noexcept -> bool
		{
			return _programs_for_frames.empty();
		}

	public:
		inline void clear() noexcept
		{
			_programs_for_frames.clear();
		}
	private:
		inline void on_window_close(window_close_parameter&)
		{
			clear();
		}
	public:

		gpu_draw_program() = default;
		template <typename TInput>
			requires std::is_constructible_v<command_type, TInput>
		gpu_draw_program(draw_pipeline& pipeline, TInput&& command) :
			_pipeline(&pipeline),
			_command(std::forward<TInput>(command)),
			_programs_for_frames(frame_data::new_frameset(pipeline, _command))
		{

		}

		gpu_draw_program(gpu_draw_program&& other) noexcept
		{
			_pipeline = std::move(other._pipeline);
			_command = std::move(other._command);
			_programs_for_frames = std::move(other._programs_for_frames);
		}
		auto operator=(gpu_draw_program&& other) noexcept -> gpu_draw_program&
		{
			clear();

			_pipeline = std::move(other._pipeline);
			_command = std::move(other._command);
			_programs_for_frames = std::move(other._programs_for_frames);

			return *this;
		}

	public:
		void draw()
		{
			frame_data::next_frame(_programs_for_frames).draw();
		}
	};

	template <typename... CommandTypes>
		requires (std::is_base_of_v<gpu_command, CommandTypes> && ...)
	auto new_gpu_program(draw_pipeline& pipeline, CommandTypes&&... command) -> gpu_draw_program<gpu_commands<CommandTypes...>>
	{
		return gpu_draw_program<gpu_commands<CommandTypes...>>(pipeline,
				gpu_commands<CommandTypes...>(
					std::forward<CommandTypes>(command)...
				)
			);
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER
