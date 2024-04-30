#include <graphics>
#include <windows>
#include <shaders>
#include <gpu_buffers>
#include <drawables>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    CompWolf::shader<> pixel_shader(environment, CompWolf::shader_code_from_file("frag.spv"));
    CompWolf::input_shader<CompWolf::float2> input_shader(environment, CompWolf::shader_code_from_file("vertex.spv"));
    CompWolf::draw_material material(input_shader, pixel_shader);
    
    CompWolf::gpu_input_buffer<CompWolf::float2> vertices(win.gpu(), {
        {-.1, .1 },
        {0, -.1 },
        {.1, .1 },
    });
    CompWolf::gpu_index_buffer indices(win.gpu(), { 0, 1, 2 });

    CompWolf::drawable triangle(win, material, indices, vertices);


    while (win.running())
    {
        win.update_image();
        environment.update();
    }
}
