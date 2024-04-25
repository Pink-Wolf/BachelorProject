#ifndef COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
#define COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER

#include <freeable>
#include "gpu_connection.hpp"
#include <utility>
#include "../graphics/graphics_environment_settings.hpp"

namespace CompWolf::Graphics
{
	class gpu_program_manager;
	struct gpu_program_manager_settings;

	/* Contains a connection to each gpu on the machine. */
	class gpu_manager : public basic_freeable
	{
	private: // fields
		/* The individual connections to each gpu. */
		std::vector<gpu_connection> _gpus;
		/* The amount of thread families the various GPUs have together. */
		std::size_t _thread_count;

	public: // accessors
		/* Returns the individual connections to each gpu. */
		inline auto& gpus() noexcept { return _gpus; }
		/* Returns the individual connections to each gpu. */
		inline auto& gpus() const noexcept { return _gpus; }

	public: // constructor
		/* Constructs a manager with no gpus. */
		gpu_manager() = default;
		gpu_manager(gpu_manager&&) = default;
		auto operator=(gpu_manager&&) -> gpu_manager& = default;
		inline ~gpu_manager() noexcept
		{
			free();
		}

		/* Sets up connections to each gpu based on the given settings.
		 * @throws std::runtime_error when something went wrong during setup outside of the program. */
		gpu_manager(const graphics_environment_settings&, Private::vulkan_instance);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _gpus.empty();
		}
		inline void free() noexcept final
		{
			_gpus.clear();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_MANAGER_HEADER
