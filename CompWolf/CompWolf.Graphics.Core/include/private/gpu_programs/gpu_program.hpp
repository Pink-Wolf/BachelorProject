#ifndef COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER
#define COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER

#include "vulkan_types"
#include "gpu_program_manager.hpp"
#include "gpu_program_code_parameters.hpp"
#include <freeable>
#include <owned>
#include <functional>

namespace CompWolf::Graphics
{
	class gpu_fence;
	class gpu_connection;

	/* Contains instructions that a gpu can run. */
	class gpu_program : public basic_freeable
	{
	public:
		owned_ptr<gpu_program_manager*> _manager;
		Private::vulkan_command _vulkan_command;

	public: // accessors
		/* Returns the manager that the program is on. */
		inline auto& manager() noexcept { return *_manager; }
		/* Returns the manager that the program is on. */
		inline auto& manager() const noexcept { return *_manager; }

		/* Returns the gpu that the program is on. */
		inline auto& device() noexcept { return manager().device(); }
		/* Returns the gpu that the program is on. */
		inline auto& device() const noexcept { return manager().device(); }

	public: // modifiers
		/* Runs the program.
		 * Returns a fence denoting when the program is finished running.
		 * @throws std::runtime_error if there was an error submitting the program to the gpu due to causes outside of the program.
		 */
		auto execute() -> gpu_fence&;

	public: // vulkan-related
		/* Returns the program's vulkan_command, representing a VkCommandBuffer. */
		inline auto vulkan_command() const noexcept { return _vulkan_command; }

	public: // constructors
		/* Constructs a freed gpu_program, as in one that cannot run. */
		gpu_program() = default;
		gpu_program(gpu_program&&) = default;
		auto operator=(gpu_program&&) -> gpu_program& = default;

		/* Creates a program on the given manager, which runs the given code.
		 * @throws std::runtime_error if there was an error during creation due to causes outside of the program.
		 */
		gpu_program(gpu_program_manager&, std::function<void(const gpu_program_code_parameters&)> code);
	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_manager;
		}
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_PROGRAM_HEADER
