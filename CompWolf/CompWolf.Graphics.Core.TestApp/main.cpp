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
    std::random_device randomizer;
    constexpr float max_random_value = static_cast<float>(std::random_device::max());

    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    std::vector<CompWolf::basic_square> squares;
    squares.reserve(1'000);
    for (std::size_t i = 0; i < 1'000; ++i)
    {
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

        for (auto& square : squares)
        {
            auto pos = square.position();
            auto delta_pos = CompWolf::float2({ randomizer() / max_random_value * 2 - 1, randomizer() / max_random_value * 2 - 1 });
            delta_pos.x() /= 100;
            delta_pos.y() /= 100;
            pos->x() += delta_pos.x();
            pos->y() += delta_pos.y();
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