#include <iostream>
#include <graphics>

using namespace CompWolf::Graphics;

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

        while (win.is_open())
        {
            environment.update();
        }

        std::cout << "Hello World!\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
        return -1;
    }
}
