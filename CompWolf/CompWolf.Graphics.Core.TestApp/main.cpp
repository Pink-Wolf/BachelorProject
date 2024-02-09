#include <iostream>
#include <graphics_environment.h>
#include <window.h>

using namespace CompWolf::Graphics::Core;

int main()
{
    graphics_environment environment;
    window win(&environment);

    while (win.is_open())
    {
        window::update();
    }

    std::cout << "Hello World!\n";
}
