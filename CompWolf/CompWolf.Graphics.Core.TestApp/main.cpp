#include <iostream>
#include <graphics>
#include <window>
#include <shader>
#include <fstream>
#include <dimensions>
#include <vector>
#include <chrono>
#include <thread>

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
        for (size_t i = 0; i < word_size; ++i)
        {
            new_data |= static_cast<shader::spv_byte_type>(new_word[i]) << (i * 8);
        }
        data.push_back(new_data);
    }

    return data;
}

struct vertex
{
    float2d position;
    float3d color;
};

template<> struct shader_field_info<vertex> : public shader_field_info_from_fields<
    type_value_pair<float2d, offsetof(vertex, position)>,
    type_value_pair<float3d, offsetof(vertex, color)>
> {};

int main()
{
    try
    {
        graphics_environment environment(graphics_environment_settings{
            .internal_debug_callback = [](std::string m) { std::cerr << m; },
            .program_name = "Test CompWolf Program",
            .program_version = {1, 2, 3},
            });
        window win(environment);

        auto vert_shader = vertex_shader<vertex>(environment, load_shader("vertex.spv"));
        auto frag_shader = shader(environment, load_shader("frag.spv"));
        auto pipeline = draw_pipeline<vertex>(draw_pipeline_settings<vertex>{
            .vertex_shader = &vert_shader,
            .fragment_shader = &frag_shader,
            }
        );

        gpu_buffer<vertex> vertices(win.device(), {
            {{+0.f, -.5f}, {1.f, 1.f, 1.f}},
            {{+.5f, +.5f}, {0.f, 1.f, 0.f}},
            {{-.5f, +.5f}, {0.f, 0.f, 1.f}}
            });

        auto draw_program = new_gpu_program(pipeline,
            draw_command(&vertices)
        );

        std::chrono::steady_clock clock;
        auto start_time = clock.now();
        auto time = clock.now();
        const std::chrono::duration<double> min_delta_time(1 / 60.);
        while (win.is_open())
        {
            {
                auto new_time = clock.now();
                auto delta_time = std::chrono::duration_cast<std::chrono::seconds>(new_time - time);
                if (delta_time < min_delta_time) std::this_thread::sleep_for(min_delta_time - delta_time);
                time = new_time;
            }

            {
                auto modifier = (std::chrono::duration_cast<std::chrono::seconds>(time - start_time).count() % 2 == 0) ? -1 : 1;

                auto vertices_data = vertices.data();
                for (auto& vertex_data : vertices_data)
                {
                    vertex_data.position.y() += static_cast<float>(min_delta_time.count() * modifier * .25);
                }
            }

            draw_program.draw(win);
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
