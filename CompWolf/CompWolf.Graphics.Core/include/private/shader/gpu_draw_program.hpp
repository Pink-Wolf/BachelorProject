#ifndef COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER

#include "vulkan_types"
#include "graphics"
#include "gpu_command.hpp"
#include "draw_pipeline.hpp"
#include <type_traits>
#include <concepts>
#include <tuple>
#include <utility>
#include <vector>
#include <optional>
#include <freeable>
#include <owned>

namespace CompWolf::Graphics
{
	namespace Private
	{
		class gpu_draw_program_frame_data : public basic_freeable
		{
		public: // fields
			using frameset_type = std::vector<gpu_draw_program_frame_data>;
		public:
			owned_ptr<draw_pipeline*> _pipeline;
			size_t _index;
			Private::vulkan_command _vulkan_command;

		public: // getters
			inline auto target_window() -> window&
			{
				return _pipeline->target_window();
			}
			inline auto target_window() const -> const window&
			{
				return _pipeline->target_window();
			}

			inline auto vulkan_command() const
			{
				return _vulkan_command;
			}
			inline auto vulkan_command_pool() const
			{
				return _pipeline->target_window().swapchain().frames()[_index].command_pool;
			}

			inline auto frame() -> swapchain_frame&
			{
				return target_window().swapchain().frames()[_index];
			}

		public: // other methods
			static auto next_frame(frameset_type&) -> gpu_draw_program_frame_data&;
			void draw();

		public: // constructors
			gpu_draw_program_frame_data() = default;
			gpu_draw_program_frame_data(gpu_draw_program_frame_data&&) = default;
			auto operator=(gpu_draw_program_frame_data&&) -> gpu_draw_program_frame_data& = default;

			gpu_draw_program_frame_data(draw_pipeline& pipeline, gpu_command& command, size_t index);

			static auto new_frameset(draw_pipeline& pipeline, gpu_command& command) -> frameset_type;

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_pipeline;
			}
			void free() noexcept final;
		};
	}

	template <typename CommandType>
		requires std::is_base_of_v<gpu_command, CommandType>
	class gpu_draw_program : public basic_freeable
	{
	public: // fields
		using command_type = CommandType;
	private:
		using frame_data = Private::gpu_draw_program_frame_data;
	private:
		draw_pipeline* _pipeline;
		command_type _command;

		frame_data::frameset_type _programs_for_frames;

		event<window_close_parameter>::key_type _window_closing_key;

	public: // getters
		inline auto target_window() -> window&
		{
			return pipeline().target_window();
		}
		inline auto target_window() const -> const window&
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

	public: // other methods
		void draw()
		{
			frame_data::next_frame(_programs_for_frames).draw();
		}

	public: // constructors
		gpu_draw_program() = default;

		template <typename TInput>
			requires std::is_constructible_v<command_type, TInput>
		gpu_draw_program(draw_pipeline& pipeline, TInput&& command) :
			_pipeline(&pipeline),
			_command(std::forward<TInput>(command)),
			_programs_for_frames(std::move(frame_data::new_frameset(pipeline, _command)))
		{
			/*
			_window_closing_key = target_window().closing.subscribe([this](event<window_close_parameter>& sender, window_close_parameter& args)
				{
					this->free();
				}
			);
			*/
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _programs_for_frames.empty();
		}
		void free() noexcept final
		{
			if (empty()) return;
			_programs_for_frames.clear();
			target_window().closing.unsubscribe(_window_closing_key);
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
