#include <graphics>
#include <windows>
using namespace CompWolf;

int main()
{
	graphics_environment environment(graphics_environment_settings{
		.program_name = "Test Program",
		});

	window win(environment, window_settings{
		.name = "Test Window",
		});

	while (win.is_open())
	{
		environment.update();
	}
}