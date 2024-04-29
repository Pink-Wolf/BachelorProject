#include <iostream>
#include <graphics>
#include <window>
#include <shaders>
#include <drawables>
#include <fstream>
#include <dimensions>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

using namespace CompWolf;
using namespace CompWolf::Graphics;

auto load_shader(std::string path) -> std::vector<uint32_t>
{
    static const auto word_size = sizeof(char);

    std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
    if (!stream.is_open()) throw std::runtime_error("Could not open " + path + "; make sure to run compile.bat to create the file.");

    std::vector<char> data;
    data.reserve(stream.tellg() * word_size);

    stream.seekg(0);
    char new_word[word_size];
    while (stream.read(new_word, word_size))
    {
        char new_data = 0;
        for (std::size_t i = 0; i < word_size; ++i)
        {
            new_data |= static_cast<char>(new_word[i]) << (i * 8);
        }
        data.push_back(new_data);
    }

    std::vector<uint32_t> output_data;
    output_data.resize(data.size() / 4);
    std::memcpy(output_data.data(), data.data(), data.size());

    return output_data;
}

struct vertex
{
    float2 position;
    float2 uv;
};
template<> struct shader_field_info<vertex> : public new_shader_field<
    type_value_pair<float2, offsetof(vertex, position)>,
    type_value_pair<float2, offsetof(vertex, uv)>
> {};

struct object_data
{
    float2 position;
};
template<> struct shader_field_info<object_data> : public new_shader_field<
    type_value_pair<float2, offsetof(vertex, position)>
> {};

int main()
{
    try
    {
        graphics_environment environment(graphics_environment_settings{
            .program_name = "Test CompWolf Program",
            .program_version = {1, 2, 3},
            .internal_debug_callback = [](std::string m) { std::cerr << m; },
            });
        window win(environment, window_settings{
            .name = "Test Window",
            }
        );

        auto vert_shader = input_shader<vertex, type_value_pair<object_data, 9>>(environment, load_shader("vertex.spv"));
        auto frag_shader = shader<type_value_pair<shader_image, 4>>(environment, load_shader("frag.spv"));
        auto material = draw_material(vert_shader, frag_shader);

        gpu_input_buffer<vertex> vertices(win.device(), {
            {{-.5f, -.5f}, {0.f, 0.f}},
            {{+.5f, -.5f}, {1.f, 0.f}},
            {{+.5f, +.5f}, {1.f, 1.f}},
            {{-.5f, +.5f}, {0.f, 1.f}}
            });
        gpu_index_buffer indices(win.device(), { 0, 1, 2, 2, 3, 0 });

        gpu_field_buffer<object_data> trans(win.device(), object_data());
        gpu_field_buffer<float> scaler(win.device(), .5f);

        gpu_image_buffer image(win.device(),
            {
                {{1,1,1,1}, {1,1,1,1}, {1,1,1,1}, {1,1,1,1}},
                {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,0,0,1}},
                {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,0,0,1}},
                {{0,0,0,1}, {0,0,0,1}, {0,0,0,1}, {0,0,0,1}}
            }
        );
        gpu_field_buffer<float> tinter(win.device(), .75f);

        auto square = drawable(win, material, indices, vertices, image, trans);

        std::chrono::steady_clock clock;
        auto start_time = clock.now();
        auto time = clock.now();

        double total_time = 0;
        std::size_t frames_since_last_print = 0;
        double next_print_time = 0;

        while (win.running())
        {
            {
                auto t = trans.single_data();

                t->position.x() = static_cast<float>(std::cos(total_time)) / 2;
                t->position.y() = static_cast<float>(std::sin(total_time)) / 2;
            }

            drawer.execute(win);
            win.update_image();
            environment.update();

            {
                time = clock.now();
                total_time = std::chrono::duration<double>(time - start_time).count();
            }

            ++frames_since_last_print;
            if (total_time >= next_print_time)
            {
                auto framerate = frames_since_last_print / (total_time - next_print_time + 1);
                std::cout << "framerate: " << framerate << "\n";
                frames_since_last_print = 0;
                next_print_time = total_time + 1;
            }
        }

        std::cout << "Hello World!\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
        throw;
    }
}
