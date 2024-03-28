#include <iostream>
#include <graphics>
#include <window>
#include <shader>
#include <fstream>
#include <dimensions>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

using namespace CompWolf;
using namespace CompWolf::Graphics;

auto load_shader(std::string path) -> std::vector<shader::spv_byte_type>
{
    static const auto word_size = sizeof(shader::spv_byte_type);

    std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
    if (!stream.is_open()) throw std::runtime_error("Could not open " + path + "; make sure to run compile.bat to create the file.");

    std::vector<shader::spv_byte_type> data;
    data.reserve(stream.tellg() * word_size);

    stream.seekg(0);
    char new_word[word_size];
    while (stream.read(new_word, word_size))
    {
        shader::spv_byte_type new_data = 0;
        for (std::size_t i = 0; i < word_size; ++i)
        {
            new_data |= static_cast<shader::spv_byte_type>(new_word[i]) << (i * 8);
        }
        data.push_back(new_data);
    }

    return data;
}

struct vertex
{
    float2 position;
    float3 color;
};
template<> struct shader_field_info<vertex> : public combine_shader_fields<
    type_value_pair<float2, offsetof(vertex, position)>,
    type_value_pair<float3, offsetof(vertex, color)>
> {};

struct transform
{
    float2 position;
};

int main()
{
    try
    {
        graphics_environment environment(graphics_environment_settings{
            .internal_debug_callback = [](std::string m) { std::cerr << m; },
            .program_name = "Test CompWolf Program",
            .program_version = {1, 2, 3},
            });
        window win(environment, window_settings{
            .name = "Test Window",
            }
        );

        auto vert_shader = vertex_shader<type_list<vertex>, type_list<transform>>(environment, load_shader("vertex.spv"));
        auto frag_shader = shader(environment, load_shader("frag.spv"));
        auto pipeline = draw_pipeline<vertex_shader<type_list<vertex>, type_list<transform>>>(vert_shader, frag_shader);

        gpu_vertex_buffer<vertex> vertices(win.device(), {
            {{-.5f, -.5f}, {1.f, 0.f, 0.f}},
            {{+.5f, -.5f}, {0.f, 1.f, 0.f}},
            {{+.5f, +.5f}, {0.f, 0.f, 1.f}},
            {{-.5f, +.5f}, {1.f, 1.f, 1.f}}
            });
        gpu_index_buffer indices(win.device(), { 0, 1, 2, 2, 3, 0 });

        gpu_uniform_buffer<transform> trans(win.device(), transform());

        auto drawer_command = draw_command<draw_pipeline<vertex_shader<type_list<vertex>, type_list<transform>>>>(
            pipeline, indices, vertices, trans
        );

        auto drawer = draw_program
        (
            draw_program_settings{
                .background = { 0, 0, 0 },
            },
            [&drawer_command](const draw_program_input& args) { return drawer_command(args); }
        );

        std::chrono::steady_clock clock;
        auto start_time = clock.now();
        auto time = clock.now();
        double total_time;
        const std::chrono::duration<double> min_delta_time(1 / 60.);
        while (win.is_open())
        {
            {
                auto new_time = clock.now();
                auto delta_time = std::chrono::duration_cast<std::chrono::seconds>(new_time - time);
                if (delta_time < min_delta_time) std::this_thread::sleep_for(min_delta_time - delta_time);
                time = new_time;
                total_time = std::chrono::duration<double>(time - start_time).count();
            }

            {
                auto transform_data = trans.data();
                auto& data = transform_data.front();

                data.position.x() = static_cast<float>(std::cos(total_time)) / 2;
                data.position.y() = static_cast<float>(std::sin(total_time)) / 2;
            }

            drawer.execute(win);
            win.update_image();
            environment.update();
        }

        std::cout << "Hello World!\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
        throw;
    }
}
