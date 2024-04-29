#include <iostream>
#include <graphics>
#include <window>
#include <shaders>
#include <drawables>
#include <dimensions>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

using namespace CompWolf;
using namespace CompWolf::Graphics;

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

        auto vert_shader = input_shader<vertex, type_value_pair<object_data, 9>>(environment, shader_code_from_file("vertex.spv"));
        auto frag_shader = shader<type_value_pair<shader_image, 4>>(environment, shader_code_from_file("frag.spv"));
        auto material = draw_material(vert_shader, frag_shader);

        gpu_input_buffer<vertex> vertices(win.gpu(), {
            {{-.5f, -.5f}, {0.f, 0.f}},
            {{+.5f, -.5f}, {1.f, 0.f}},
            {{+.5f, +.5f}, {1.f, 1.f}},
            {{-.5f, +.5f}, {0.f, 1.f}}
            });
        gpu_index_buffer indices(win.gpu(), { 0, 1, 2, 2, 3, 0 });

        gpu_field_buffer<object_data> trans(win.gpu(), object_data());
        gpu_field_buffer<float> scaler(win.gpu(), .5f);

        gpu_image_buffer image(win.gpu(),
            {
                {{1,1,1,1}, {1,1,1,1}, {1,1,1,1}, {1,1,1,1}},
                {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,0,0,1}},
                {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,0,0,1}},
                {{0,0,0,1}, {0,0,0,1}, {0,0,0,1}, {0,0,0,1}}
            }
        );
        gpu_field_buffer<float> tinter(win.gpu(), .75f);

        auto square = drawable(win, material, indices, vertices, image, trans);

        std::chrono::steady_clock clock;
        auto start_time = clock.now();
        auto time = clock.now();

        double real_total_time = 0;
        double real_delta_time = 1;
        double total_time = 0;
        double delta_time = 1;
        std::size_t frames_since_last_print = 0;
        double next_print_time = 0;

        double time_multiplier = 1;

        win.inputs().char_pressed('w').subscribe([&time_multiplier](key_parameter& args)
            {
                time_multiplier *= 2;
            }
        );
        win.inputs().char_pressed('s').subscribe([&time_multiplier](key_parameter& args)
            {
                time_multiplier /= 2;
            }
        );
        win.inputs().char_released().subscribe([](key_parameter& args)
            {
                std::cout << args.character << "\n";
            }
        );

        while (win.running())
        {
            {
                auto t = trans.single_data();

                t->position.x() = static_cast<float>(std::cos(total_time)) / 2;
                t->position.y() = static_cast<float>(std::sin(total_time)) / 2;
            }

            win.update_image();
            environment.update();

            {
                auto new_time = clock.now();;
                
                real_delta_time = std::chrono::duration<double>(new_time - time).count();
                real_total_time = real_total_time += real_delta_time;

                delta_time = real_delta_time * time_multiplier;
                total_time = total_time += delta_time;
                
                time = new_time;
            }

            ++frames_since_last_print;
            if (real_total_time >= next_print_time)
            {
                auto framerate = frames_since_last_print / (real_total_time - next_print_time + 1);
                std::cout << "framerate: " << framerate << "\n";
                frames_since_last_print = 0;
                next_print_time = real_total_time + 1;
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
