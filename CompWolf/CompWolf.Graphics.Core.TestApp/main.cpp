#include <graphics>
#include <windows>
#include <shaders>
#include <gpu_buffers>
#include <drawables>
#include <basic_drawables>
#include <iostream>

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    auto environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    CompWolf::basic_square square(win);

    {
        auto pos = square.position().single_data();
        *pos = { .5f, .0f };
    }

    while (win.running())
    {
        win.update_image();
        environment.update();
    }
}
