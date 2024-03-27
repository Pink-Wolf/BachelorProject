#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER

#include "vulkan_types"
#include <freeable>
#include <functional>
#include <owned>

namespace CompWolf::Graphics
{
	struct gpu_program_compile_parameter
	{
		Private::vulkan_command command;
	};
	using gpu_program_compile_function = std::function<void(const gpu_program_compile_parameter&)>;

	class gpu_program : public basic_freeable
	{
	public:
		owned_ptr<gpu*> _device;
		command_pool* _command_pool;
		Private::vulkan_command _vulkan_command;

	public: // getters
		inline auto device() -> gpu& { return *_device; }
		inline auto device() const -> const gpu& { return *_device; }

		inline auto pool() -> command_pool& { return *_command_pool; }
		inline auto pool() const -> const command_pool& { return *_command_pool; }

		inline auto vulkan_command() const { return _vulkan_command; }

	public: // other methods
		auto execute() -> gpu_fence&;

	public: // constructors
		gpu_program() = default;
		gpu_program(gpu_program&&) = default;
		auto operator=(gpu_program&&)->gpu_program & = default;

		gpu_program(gpu& target_device
			, command_pool& pool
			, gpu_program_compile_function code
		);
	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_device;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER