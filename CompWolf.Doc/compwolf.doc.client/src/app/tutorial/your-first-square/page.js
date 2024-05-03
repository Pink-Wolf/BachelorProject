import { CodeViewer } from "@/lib/CodeComponents";
import FormattedText from "@/lib/FormattedText";
import Link from "next/link";

export default function YourFirstSquare() {
    return (
        <div>
            <h1>Your First Square</h1>
            This tutorial builds ontop of <Link href={"/tutorial/your-first-window"}>your-first-window</Link>; it is recommended to first complete that tutorial.
            <FormattedText>
                This tutorial will go over how to make a square be displayed on your window, and change its color.
            </FormattedText>
            <h2>basic_square</h2>
            <FormattedText>
                The type of objects that are displayed on a window inherits from [[drawable]].
                One of these types that [[Graphics.Core]] provides is [[basic_square]].
                Constructing a [[drawable]], like [[basic_square]], requires providing what window it is displayed on.
                [[basic_square]]s are defined in the [[basic_drawables]]-header. Write the following code to create a [[basic_square]]:
            </FormattedText>
            <CodeViewer>
                {`#include <basic_drawables>`}
            </CodeViewer>
            <CodeViewer>
                {`CompWolf::basic_square square(win);`}
            </CodeViewer>
            <FormattedText>
                If we run the program now, we will not see any square. This is because we need to tell the window to update what it displays on screen with win.update_image().
                In the while loop, write:
            </FormattedText>
            <CodeViewer>
                {`win.update_image();`}
            </CodeViewer>
            <FormattedText>
                If we run the program now, the square should be visible.
            </FormattedText>
            <h2>Red square</h2>
            <FormattedText>
                [[drawable]] objects may have some buffers with for example their position or color. A [[basic_square]] has both.
                The square&apos;s color can be gotten with its method color(). This returns a smart-pointer (a pointer that automatically frees its access when destroyed) to the color.
                The pointer must be destroyed to allow the gpu to read the new color of the square.
                The square&apos;s color is made out of red, green, and blue; so to set the color to for example red, we must set its color to (1, 0, 0).
                The square is currently just white. Get the color and change it to red. Do this inside a pair of curly brackets so the smart pointer is destroyed afterwards:
            </FormattedText>
            <CodeViewer>
                {`{
    auto color = square.color();
    *color = { 1, 0, 0 };
}`}
            </CodeViewer>
            <FormattedText>
                Congratulations, you have now created a red square on your window.
            </FormattedText>
            <h2>Final code</h2>
            <CodeViewer>
                {`#include <graphics>
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
}`}
            </CodeViewer>
        </div>
    );
}
