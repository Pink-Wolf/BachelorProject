#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER

#include "vulkan_types"
#include <freeable>
#include <functional>
#include <owned>

namespace CompWolf::Graphics
{
	class gpu_program_pool;
	class gpu_fence;

	struct gpu_program_input
	{
		Private::vulkan_command command;
	};
	using gpu_program_code = std::function<void(const gpu_program_input&)>;

	class gpu_program : public basic_freeable
	{
	public:
		owned_ptr<gpu_connection*> _device;
		gpu_program_pool* _job;
		Private::vulkan_command _vulkan_command;

	public: // getters
		inline auto device() -> gpu_connection& { return *_device; }
		inline auto device() const -> const gpu_connection& { return *_device; }

		inline auto& job() { return *_job; }
		inline auto& job() const { return *_job; }

		inline auto vulkan_command() const { return _vulkan_command; }

	public: // other methods
		auto execute() -> gpu_fence&;

	public: // constructors
		gpu_program() = default;
		gpu_program(gpu_program&&) = default;
		auto operator=(gpu_program&&) -> gpu_program& = default;

		gpu_program(gpu_connection& target_device
			, gpu_program_pool& job
			, gpu_program_code code
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
