#ifndef COMPWOLF_GRAPHICS_DRAW_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_DRAW_PROGRAM_HEADER

#include "vulkan_types"
#include "gpu_programs"
#include "window"
#include <functional>
#include <dimensions>
#include <utility>
#include <concepts>
#include <map>
#include <vector>

namespace CompWolf::Graphics
{
	struct draw_program_input : public gpu_program_code_parameters
	{
		window* target_window;
		std::size_t frame_index;
	};
	using draw_program_code = std::function<void(const draw_program_input&)>;

	struct draw_program_settings
	{
		float3 background;
	};

	namespace Private
	{
		struct draw_program_data
		{
			draw_program_settings settings;
			draw_program_code code;
		};

		class draw_frame_program
		{
		private: // fields
			window* _target_window;
			draw_program_data* _data;
			std::size_t _frame_index;

			gpu_program _program;

		public: // getters
			inline auto target_window() noexcept -> window& { return *_target_window; }
			inline auto target_window() const noexcept -> const window& { return *_target_window; }

			inline auto frame_index() const noexcept { return _frame_index; }

		public: // other methods
			auto execute() -> gpu_fence& { return _program.execute(); }
		private:
			void gpu_code(const gpu_program_code_parameters&);
			
		public: // constructors
			draw_frame_program() = default;
			draw_frame_program(draw_frame_program&&) = default;
			auto operator=(draw_frame_program&&) -> draw_frame_program& = default;

			draw_frame_program(draw_program_data& data, window& target, std::size_t frame_index);
		};

		class draw_window_program
		{
		private: // fields
			window* _target_window;
			draw_program_data* _data;
			std::vector<draw_frame_program> _frame_programs;

		public: // getters
			inline auto target_window() noexcept -> window& { return *_target_window; }
			inline auto target_window() const noexcept -> const window& { return *_target_window; }

			inline auto frame_program(std::size_t frame_index) noexcept -> draw_frame_program& { return _frame_programs[frame_index]; }
			inline auto frame_program(std::size_t frame_index) const noexcept -> const draw_frame_program& { return _frame_programs[frame_index]; }
			
			inline auto current_frame_program() noexcept -> draw_frame_program&
			{ return frame_program(target_window().swapchain().current_frame_index()); }
			inline auto current_frame_program() const noexcept -> const draw_frame_program&
			{ return frame_program(target_window().swapchain().current_frame_index()); }

		public: // operators
			auto execute() -> gpu_fence& { return current_frame_program().execute(); }

		public: // constructors
			draw_window_program() = default;
			draw_window_program(draw_window_program&&) = default;
			auto operator=(draw_window_program&&) -> draw_window_program& = default;

			draw_window_program(draw_program_data& data, window& target);
		};
	}

	class draw_program
	{
	private: // fields
		Private::draw_program_data _data;

		std::map<window*, Private::draw_window_program> _window_commands;

	public: // getters
		auto window_program(window&) noexcept -> Private::draw_window_program&;
		inline auto window_program(window& target) const noexcept -> const Private::draw_window_program&
		{
			const_cast<draw_program*>(this)->window_program(target);
		}

	public: // operators
		inline auto execute(window& target) -> gpu_fence& { return window_program(target).execute(); }

	public: // constructors
		draw_program() = default;
		draw_program(draw_program&&) = default;
		auto operator=(draw_program&&) -> draw_program& = default;

		draw_program(draw_program_settings& settings, draw_program_code&& code)
			: _data{ settings, std::move(code) }
		{}
		draw_program(draw_program_settings&& settings, draw_program_code&& code)
			: _data{ std::move(settings), std::move(code) }
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_PROGRAM_HEADER
