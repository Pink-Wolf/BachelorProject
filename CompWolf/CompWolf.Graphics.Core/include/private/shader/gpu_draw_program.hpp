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
#include <map>

namespace CompWolf::Graphics
{
	namespace Private
	{
		class gpu_draw_program_frame_data : public basic_freeable
		{
		public: // fields
			using frameset_type = std::vector<gpu_draw_program_frame_data>;
		public:
			owned_ptr<window_specific_pipeline*> _pipeline;
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
				return target_window().swapchain().frames()[_index].command_pool;
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

			gpu_draw_program_frame_data(window_specific_pipeline& pipeline, gpu_command* command, size_t index);

			static auto new_frameset(window_specific_pipeline& pipeline, gpu_command* command) -> frameset_type;

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_pipeline;
			}
			void free() noexcept final;
		};

		class window_draw_program : public basic_freeable
		{
		private:
			window_specific_pipeline* _pipeline;
			using frame_data = Private::gpu_draw_program_frame_data;
			frame_data::frameset_type _programs_for_frames;
			event<window_close_parameter>::key_type _window_closing_key;

		public: // getters
			inline auto pipeline() noexcept -> window_specific_pipeline&
			{
				return *_pipeline;
			}
			inline auto pipeline() const noexcept -> const window_specific_pipeline&
			{
				return *_pipeline;
			}

			inline auto target_window() noexcept -> window&
			{
				return pipeline().target_window();
			}
			inline auto target_window() const noexcept -> const window&
			{
				return pipeline().target_window();
			}

		public: // other methods
			inline void draw()
			{
				frame_data::next_frame(_programs_for_frames).draw();
			}

		public: // constructors
			window_draw_program() = default;
			window_draw_program(window_draw_program&&) = default;
			auto operator=(window_draw_program&&) -> window_draw_program& = default;

			window_draw_program(window_specific_pipeline& pipeline, gpu_command* command)
				: _pipeline(&pipeline)
				, _window_closing_key(target_window().closing.subscribe(
					[this](event<window_close_parameter>& sender, window_close_parameter& args) { free(); }
				))
				, _programs_for_frames(frame_data::new_frameset(pipeline, command)) {}

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

		std::map<window*, Private::window_draw_program> _window_data;

	public: // getters
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
		void draw(window& target_window)
		{
			auto& window_data = _window_data.try_emplace(&target_window, pipeline().window_data(target_window), &command()).first->second;
			window_data.draw();
		}

	public: // constructors
		gpu_draw_program() = default;

		template <typename TInput>
			requires std::is_constructible_v<command_type, TInput>
		gpu_draw_program(draw_pipeline& pipeline, TInput&& command) :
			_pipeline(&pipeline),
			_command(std::forward<TInput>(command))
		{}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _window_data.empty();
		}
		inline void free() noexcept final
		{
			_window_data.clear();
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
