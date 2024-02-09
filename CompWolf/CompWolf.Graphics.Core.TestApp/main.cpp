#include <iostream>
#include <graphics_environment.h>
#include <window.h>

using namespace CompWolf::Graphics;

int main()
{
    graphics_environment environment;
    window win(environment);

    while (win.is_open())
    {
        environment.update();
    }

    std::cout << "Hello World!\n";
}
