#ifndef COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER

#include "vulkan_types"
#include "graphics"
#include "window"
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
			static inline auto current_frame(frameset_type& frameset) -> gpu_draw_program_frame_data&
			{
				auto& target_window = frameset[0]._pipeline->target_window();
				auto index = target_window.swapchain().current_frame_index();
				return frameset[index];
			}
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

		class window_draw_program : public basic_freeable, private free_on_dependent_freed<window_specific_pipeline>
		{
		private:
			gpu_command* _command;
			using frame_data = Private::gpu_draw_program_frame_data;
			frame_data::frameset_type _programs_for_frames;

		public: // getters
			inline auto pipeline() noexcept -> window_specific_pipeline&
			{
				window_specific_pipeline* p;
				get_dependent(p);
				return *p;
			}
			inline auto pipeline() const noexcept -> const window_specific_pipeline&
			{
				const window_specific_pipeline* p;
				get_dependent(p);
				return *p;
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
				frame_data::current_frame(_programs_for_frames).draw();
			}

		public: // constructors
			window_draw_program() = default;
			window_draw_program(window_draw_program&&) = default;
			auto operator=(window_draw_program&&) -> window_draw_program& = default;

			window_draw_program(window_specific_pipeline& pipeline, gpu_command* command)
				: free_on_dependent_freed<window_specific_pipeline>(pipeline)
				, _command(command)
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
				set_dependent(nullptr);
			}
		};
	}

	template <ShaderField InputType, typename CommandType>
		requires std::is_base_of_v<gpu_command, CommandType>
	class gpu_draw_program : public basic_freeable
	{
	public: // fields
		using command_type = CommandType;
	private:
		using frame_data = Private::gpu_draw_program_frame_data;
	private:
		draw_pipeline<InputType>* _pipeline;
		command_type _command;

		std::map<window*, Private::window_draw_program> _window_data;

	public: // getters
		inline auto pipeline() noexcept -> draw_pipeline<InputType>&
		{
			return *_pipeline;
		}
		inline auto pipeline() const noexcept -> const draw_pipeline<InputType>&
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

	private:
		auto get_window_data(window& target_window) -> Private::window_draw_program&
		{
			auto iterator = _window_data.find(&target_window);
			if (iterator != _window_data.end())
			{
				auto& window_data = iterator->second;
				if (window_data.empty()) _window_data.erase(iterator);
				else return window_data;
			}
			return _window_data.try_emplace(
				&target_window,
				pipeline().window_data(target_window),
				&command()
			).first->second;
		}

	public: // other methods
		inline void draw(window& target_window)
		{
			get_window_data(target_window).draw();
		}

	public: // constructors
		gpu_draw_program() = default;

		template <typename TInput>
			requires std::is_constructible_v<command_type, TInput>
		gpu_draw_program(draw_pipeline<InputType>& pipeline, TInput&& command) :
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

	template <ShaderField InputType, typename... CommandTypes>
		requires (std::is_base_of_v<gpu_command, CommandTypes> && ...)
	auto new_gpu_program(draw_pipeline<InputType>& pipeline, CommandTypes&&... command)
	{
		return gpu_draw_program<InputType, gpu_commands<CommandTypes...>>(pipeline,
				gpu_commands<CommandTypes...>(
					std::forward<CommandTypes>(command)...
				)
			);
	}
}

#endif // ! COMPWOLF_GRAPHICS_GPU_DRAW_PROGRAM_HEADER
