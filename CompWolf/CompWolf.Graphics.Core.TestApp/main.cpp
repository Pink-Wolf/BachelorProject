#include <iostream>
#include <graphics>
#include <window>
#include <draw_program>
#include <fstream>
#include <dimensions>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

using namespace CompWolf;
using namespace CompWolf::Graphics;

auto load_shader(std::string path) -> shader_code_type
{
    static const auto word_size = sizeof(shader_code_char_type);

    std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
    if (!stream.is_open()) throw std::runtime_error("Could not open " + path + "; make sure to run compile.bat to create the file.");

    shader_code_type data;
    data.reserve(stream.tellg() * word_size);

    stream.seekg(0);
    char new_word[word_size];
    while (stream.read(new_word, word_size))
    {
        shader_code_char_type new_data = 0;
        for (std::size_t i = 0; i < word_size; ++i)
        {
            new_data |= static_cast<shader_code_char_type>(new_word[i]) << (i * 8);
        }
        data.push_back(new_data);
    }

    return data;
}

struct vertex
{
    float2 position;
    float2 uv;
};
template<> struct shader_field_info<vertex> : public combine_shader_fields<
    type_value_pair<float2, offsetof(vertex, position)>,
    type_value_pair<float2, offsetof(vertex, uv)>
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
            .program_name = "Test CompWolf Program",
            .program_version = {1, 2, 3},
            .internal_debug_callback = [](std::string m) { std::cerr << m; },
            });
        window win(environment, window_settings{
            .name = "Test Window",
            }
        );

        auto vert_shader = vertex_shader<vertex, type_list<transform>>(environment, load_shader("vertex.spv"));
        auto frag_shader = shader<type_list<shader_image>>(environment, load_shader("frag.spv"));
        auto pipeline = new_draw_pipeline(vert_shader, frag_shader);

        gpu_vertex_buffer<vertex> vertices(win.device(), {
            {{-.5f, -.5f}, {0.f, 0.f}},
            {{+.5f, -.5f}, {1.f, 0.f}},
            {{+.5f, +.5f}, {1.f, 1.f}},
            {{-.5f, +.5f}, {0.f, 1.f}}
            });
        gpu_index_buffer indices(win.device(), { 0, 1, 2, 2, 3, 0 });

        gpu_uniform_buffer<transform> trans(win.device(), transform());
        gpu_uniform_buffer<float> scaler(win.device(), .5f);

        gpu_image_buffer image(win.device(),
            {
                {{255,255,255,255}, {255,255,255,255},  {255,255,255,255},  {255,255,255,255}},
                {{255,0,0,255},     {0,255,0,255},      {0,0,255,255},      {255,0,0,255}},
                {{255,0,0,255},     {0,255,0,255},      {0,0,255,255},      {255,0,0,255}},
                {{0,0,0,255},       {0,0,0,255},        {0,0,0,255},        {0,0,0,255}}
            }
        );
        gpu_uniform_buffer<float> tinter(win.device(), .75f);

        auto drawer_command = new_draw_command(pipeline
            , indices, vertices
            , trans
            , image);

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
        const std::chrono::duration<double> min_delta_time(1 / 60.);

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
                auto new_time = clock.now();
                auto delta_time = std::chrono::duration_cast<std::chrono::duration<double>>(new_time - time);
                if (delta_time < min_delta_time) std::this_thread::sleep_for(min_delta_time - delta_time);
                time = new_time;
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
