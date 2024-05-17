#include <graphics>
#include <windows>
#include <basic_drawables>

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <ppl.h>

CompWolf::graphics_environment environment;

struct vertex
{
    CompWolf::float2 position;
    CompWolf::float3 color;
};
template<> struct CompWolf::shader_field_info<vertex> : public CompWolf::new_shader_field<
    CompWolf::type_value_pair<CompWolf::float2, offsetof(vertex, position)>,
    CompWolf::type_value_pair<CompWolf::float3, offsetof(vertex, color)>
> {};

int main()
{
    std::random_device randomizer;
    constexpr float max_random_value = static_cast<float>(std::random_device::max());

    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    static constexpr std::size_t square_count = 100;

    CompWolf::gpu_index_buffer shapes_indices(win.gpu(), square_count * 6);
    {
        auto indices = shapes_indices.data();
        for (std::size_t i = 0; i < indices.size(); i += 6)
        {
            auto vertexI = static_cast<CompWolf::shader_int>(i / 6 * 4);
            indices[i + 0] = vertexI + 0;
            indices[i + 1] = vertexI + 1;
            indices[i + 2] = vertexI + 2;
            indices[i + 3] = vertexI + 3;
            indices[i + 4] = vertexI + 0;
            indices[i + 5] = vertexI + 2;
        }
    }
    CompWolf::gpu_input_buffer<vertex> shapes_vertices(win.gpu(), square_count * 4);
    {
        auto vertices = shapes_vertices.data();
        for (std::size_t i = 0; i < vertices.size(); i += 4)
        {
            CompWolf::float2 center = { randomizer() / max_random_value * 2 - 1, randomizer() / max_random_value * 2 - 1 };
            CompWolf::float3 color = { randomizer() / max_random_value, randomizer() / max_random_value, randomizer() / max_random_value };

            vertices[i + 0].position = { center.x() - .1f, center.y() - .1f };
            vertices[i + 1].position = { center.x() + .1f, center.y() - .1f };
            vertices[i + 2].position = { center.x() + .1f, center.y() + .1f };
            vertices[i + 3].position = { center.x() - .1f, center.y() + .1f };

            vertices[i + 0].color = color;
            vertices[i + 1].color = color;
            vertices[i + 2].color = color;
            vertices[i + 3].color = color;
        }
    }
    static constexpr const char input_shader_path[] = "vertex.spv";
    CompWolf::static_shader<input_shader_path, CompWolf::input_shader<vertex>> input_shader(true);
    static constexpr const char pixel_shader_path[] = "frag.spv";
    CompWolf::static_shader<pixel_shader_path, CompWolf::shader<>> pixel_shader(true);
    CompWolf::draw_material shape_material(input_shader.shader(), pixel_shader.shader());

    CompWolf::drawable square_shapes(win, shape_material, shapes_indices, shapes_vertices);

    std::chrono::steady_clock clock;
    auto start_time = clock.now();
    auto time = clock.now();
    double total_time = 0;

    double next_framerate_time = 1;
    int framerate_count = 0;

    std::vector<float> delta_positions;
    delta_positions.resize(square_count * 2);

    while (win.running())
    {
        {
            time = clock.now();
            total_time = std::chrono::duration<double>(time - start_time).count();
        }

        {
            auto vertices = shapes_vertices.data();
            auto vertices_ptr = vertices.data();

            for (auto& p : delta_positions) p = static_cast<float>(randomizer());
            auto delta_positions_ptr = delta_positions.data();

            concurrency::parallel_for(std::size_t(0), vertices.size() / 4, [vertices_ptr, delta_positions_ptr](std::size_t i)
                {
                    auto delta_pos = CompWolf::float2({ delta_positions_ptr[i * 2 + 0] / max_random_value * 2 - 1, delta_positions_ptr[i * 2 + 1] / max_random_value * 2 - 1 });
                    delta_pos.x() /= 100;
                    delta_pos.y() /= 100;

                    auto sqrI = i * 4;

                    vertices_ptr[sqrI + 0].position.x() += delta_pos.x();
                    vertices_ptr[sqrI + 0].position.y() += delta_pos.y();

                    vertices_ptr[sqrI + 1].position.x() += delta_pos.x();
                    vertices_ptr[sqrI + 1].position.y() += delta_pos.y();

                    vertices_ptr[sqrI + 2].position.x() += delta_pos.x();
                    vertices_ptr[sqrI + 2].position.y() += delta_pos.y();

                    vertices_ptr[sqrI + 3].position.x() += delta_pos.x();
                    vertices_ptr[sqrI + 3].position.y() += delta_pos.y();
                });
        }

        win.update_image();
        environment.update();

        ++framerate_count;
        if (total_time >= next_framerate_time)
        {
            std::cout << (framerate_count / (total_time - next_framerate_time + 1)) << "\n";

            next_framerate_time = total_time + 1;
            framerate_count = 0;
        }
    }
}