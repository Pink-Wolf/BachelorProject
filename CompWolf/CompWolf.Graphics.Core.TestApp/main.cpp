#include <graphics>
#include <windows>
#include <basic_drawables>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    CompWolf::basic_square square(win);

    {
        auto color = square.color();
        *color = { 1, 0, 0 };
    }

    while (win.running())
    {
        win.update_image();
        environment.update();
    }
}