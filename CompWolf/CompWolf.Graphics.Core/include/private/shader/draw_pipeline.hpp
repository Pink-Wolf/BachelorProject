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

	class window_specific_pipeline : public basic_freeable
	{
	private: // fields
		owned_ptr<window*> _target_window;
		Private::vulkan_render_pass _render_pass;
		Private::vulkan_pipeline _pipeline;
		std::vector<Private::vulkan_frame_buffer> _frame_buffers;

	public: // getters
		auto target_window() noexcept -> window& { return *_target_window; }
		auto target_window() const noexcept -> const window& { return *_target_window; }

		auto vulkan_render_pass() const noexcept -> Private::vulkan_render_pass { return _render_pass; }
		auto vulkan_pipeline() const noexcept -> Private::vulkan_pipeline { return _pipeline; }
		auto vulkan_frame_buffer(size_t index) const noexcept -> Private::vulkan_frame_buffer { return _frame_buffers[index]; }

	public: // constructor
		window_specific_pipeline() = default;
		window_specific_pipeline(window_specific_pipeline&&) = default;
		auto operator=(window_specific_pipeline&&)->window_specific_pipeline & = default;
		inline ~window_specific_pipeline() noexcept { free(); }

		window_specific_pipeline(window&, const draw_pipeline_settings&, const Private::gpu_specific_pipeline&);

	public: // CompWolf::freeable
		inline auto empty() const noexcept -> bool final
		{
			return !_target_window;
		}
		void free() noexcept final;
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
			return !_gpu_data.empty();
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
