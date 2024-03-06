#ifndef COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER
#define COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER

#include "vulkan_types"
#include <tuple>
#include <compwolf_type_traits>

namespace CompWolf::Graphics
{
	struct gpu_command
	{
	public:
		virtual void compile(Private::vulkan_command) = 0;
	};

	namespace Private
	{
		template <typename T>
		struct compile_gpu_command_function
		{
			static void invoke(T& command, Private::vulkan_command vulkan_command)
			{
				command.compile(vulkan_command);
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
		inline void compile(Private::vulkan_command a) final
		{
			for_each_in<std::tuple<TCommands...>>::invoke<Private::compile_gpu_command_function>(*static_cast<std::tuple<TCommands...>*>(this), a);
		}
	};

	struct draw_command : public gpu_command
	{
		void compile(Private::vulkan_command) final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_COMMAND_HEADER
