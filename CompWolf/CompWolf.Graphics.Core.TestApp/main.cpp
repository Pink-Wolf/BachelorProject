#include <graphics>
#include <windows>
#include <basic_drawables>

#include <iostream>
#include <vector>
#include <chrono>
#include <random>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    std::vector<CompWolf::basic_square> squares;
    squares.reserve(1000);
    std::random_device randomizer;
    for (std::size_t i = 0; i < 1000; ++i)
    {
        float max_random_value = static_cast<float>(std::random_device::max());
        squares.emplace_back(win
            , CompWolf::float3({ randomizer() / max_random_value, randomizer() / max_random_value, randomizer() / max_random_value })
            , CompWolf::float2({ randomizer() / max_random_value * 2 - 1, randomizer() / max_random_value * 2 - 1 }));
    }

    std::chrono::steady_clock clock;
    auto start_time = clock.now();
    auto time = clock.now();
    double total_time = 0;

    double next_framerate_time = 1;
    int framerate_count = 0;

    while (win.running())
    {
        {
            time = clock.now();
            total_time = std::chrono::duration<double>(time - start_time).count();
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