#ifndef COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER
#define COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER

#include "vulkan_types"
#include <tuple>
#include <compwolf_type_traits>
#include "gpu_buffer.hpp"
#include <type_traits>

namespace CompWolf::Graphics
{
	struct gpu_command_compile_settings
	{
		Private::vulkan_command command;
		window_specific_pipeline* pipeline;
		size_t frame_index;
	};

	struct gpu_command
	{
	public:
		virtual void compile(const gpu_command_compile_settings&) = 0;
	};

	namespace Private
	{
		template <typename T>
		struct compile_gpu_command_function
		{
			static void invoke(T& command, const gpu_command_compile_settings& s)
			{
				command.compile(s);
			}
		};
	}
	template <typename... TCommands>
		requires (std::is_base_of_v<gpu_command, TCommands> && ...)
	struct gpu_commands : public std::tuple<TCommands...>, gpu_command
	{
		using std::tuple<TCommands...>::tuple;
	public:
		using command_types = type_list<TCommands...>;

	public:
		inline void compile(const gpu_command_compile_settings& s) final
		{
			for_each_in<std::tuple<TCommands...>>
				::template invoke<Private::compile_gpu_command_function>
				(*static_cast<std::tuple<TCommands...>*>(this), s);
		}
	};

	namespace Private
	{
		struct base_draw_command : public gpu_command
		{
		protected: // fields
			base_gpu_buffer* base_buffer;
		public: // CompWolf::Graphics::gpu_command
			void compile(const gpu_command_compile_settings&) final;
		};
	}
	template <typename InputType>
	struct draw_command : public Private::base_draw_command
	{
	private: // fields
		gpu_buffer<InputType>* _buffer;
	public: // getters
		inline auto buffer() noexcept -> gpu_buffer<InputType>& { return *_buffer; }
		inline auto buffer() const noexcept -> const gpu_buffer<InputType>& { return *_buffer; }

	public: // constructor
		draw_command() = default;
		draw_command(const draw_command&) = default;
		auto operator=(const draw_command&) -> draw_command& = default;
		draw_command(draw_command&&) = default;
		auto operator=(draw_command&&) -> draw_command& = default;

		draw_command(gpu_buffer<InputType>* buffer) : _buffer(buffer)
		{
			base_buffer = buffer;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER
