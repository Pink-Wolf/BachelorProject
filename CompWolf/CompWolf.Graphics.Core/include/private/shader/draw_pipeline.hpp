#ifndef COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
#define COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER

#include "vulkan_types"
#include "graphics"
#include "window"
#include "shader.hpp"
#include <vector>
#include <owned>
#include <freeable>
#include <map>
#include <free_on_dependent_freed>

namespace CompWolf::Graphics
{
	struct draw_pipeline_settings
	{
		shader* vertex_shader;
		shader* fragment_shader;
	};

	namespace Private
	{
		class gpu_specific_pipeline : public basic_freeable
		{
		private: // fields
			owned_ptr<gpu*> _device;
			Private::vulkan_pipeline_layout _layout;

		public: // getters
			auto device() noexcept -> gpu& { return *_device; }
			auto device() const noexcept -> const gpu& { return *_device; }

			auto layout() const noexcept -> Private::vulkan_pipeline_layout { return _layout; }

		public: // constructor
			gpu_specific_pipeline() = default;
			gpu_specific_pipeline(gpu_specific_pipeline&&) = default;
			auto operator=(gpu_specific_pipeline&&)->gpu_specific_pipeline & = default;
			inline ~gpu_specific_pipeline() noexcept { free(); }

			gpu_specific_pipeline(gpu&, const draw_pipeline_settings&);

		public: // CompWolf::freeable
			inline auto empty() const noexcept -> bool final
			{
				return !_device;
			}
			void free() noexcept final;
		};
	}

	class window_specific_pipeline : public basic_freeable, public window_user
	{
	private: // fields
		const draw_pipeline_settings* _settings;
		const Private::gpu_specific_pipeline* _gpu_data;

		Private::vulkan_render_pass _render_pass;
		Private::vulkan_pipeline _pipeline;
		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

	protected: // event handlers
		void on_swapchain_rebuild(
			const event<window_rebuild_swapchain_parameter>&,
			window_rebuild_swapchain_parameter&
		) final;

	public: // getters
		inline auto vulkan_render_pass() const noexcept -> Private::vulkan_render_pass { return _render_pass; }
		inline auto vulkan_pipeline() const noexcept -> Private::vulkan_pipeline { return _pipeline; }
		inline auto vulkan_frame_buffer(size_t index) const noexcept -> Private::vulkan_frame_buffer { return _frame_buffers[index]; }

	private: // constructor
		void setup();
	public: // constructor
		window_specific_pipeline() = default;
		window_specific_pipeline(window_specific_pipeline&&) = default;
		auto operator=(window_specific_pipeline&&)->window_specific_pipeline & = default;
		inline ~window_specific_pipeline() noexcept { free(); }

		inline window_specific_pipeline(window& target_window, const draw_pipeline_settings& settings, const Private::gpu_specific_pipeline& gpu_data)
			: window_user(target_window)
			, _settings(&settings)
			, _gpu_data(&gpu_data)
		{
			if (&target_window.device() != &gpu_data.device())
				throw std::invalid_argument("Tried creating window-specific pipeline logic with gpu-data for a gpu other than the window's");
			setup();
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !has_window();
		}
		void free() noexcept final;
		event<> freeing;
	};

	class draw_pipeline : public basic_freeable
	{
	private: // fields
		draw_pipeline_settings _settings;

		mutable std::map<gpu*, Private::gpu_specific_pipeline> _gpu_data;

		mutable std::map<window*, window_specific_pipeline> _window_data;

	public: // getters
		auto window_data(window&) const noexcept -> const window_specific_pipeline&;
		inline auto window_data(window& a) noexcept -> window_specific_pipeline&
		{
			return const_cast<window_specific_pipeline&>(const_cast<const draw_pipeline*>(this)->window_data(a));
		}

	private: // constructors
		void setup();
	public:
		draw_pipeline() = default;
		draw_pipeline(draw_pipeline&&) = default;
		auto operator =(draw_pipeline&&) -> draw_pipeline& = default;
		inline ~draw_pipeline() noexcept
		{
			free();
		}

		template<typename T>
			requires std::convertible_to<T, draw_pipeline_settings>
		draw_pipeline(T&& settings) : _settings(std::forward<draw_pipeline_settings>(settings))
		{
			setup();
		}

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return _gpu_data.empty();
		}
		void free() noexcept final
		{
			if (empty()) return;

			_window_data.clear();
			_gpu_data.clear();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAW_PIPELINE_HEADER
