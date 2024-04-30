import { CodeViewer, SimpleReference } from "@/lib/CodeComponents";
import FormattedText from "@/lib/FormattedText";
import Link from "next/link";

export default function Home() {
    return (
        <div>
            <h1>Your First Window</h1>
            <FormattedText>This tutorial will go over how to create a window using the library [[CompWolf.Graphics.Core]].</FormattedText>
            <h2>graphics_environment</h2>
            <FormattedText>
                Most features of CompWolf.Graphics requires a [[graphics_environment]] to work. The [[graphics_environment]] is defined in the [[graphics]]-header.
                Creating an environment requires a [[graphics_environment_settings]], which can be used to adjust how the environment functions. For this tutorial, we can just let it function as normal.
                To create a program that creates a [[graphics_environment]], write the following code:
            </FormattedText>
            <CodeViewer>
                {`#include <graphics>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);
}`}
            </CodeViewer>
            <h2>window</h2>
            <FormattedText>
                With a [[graphics_environment]], you can now create a [[window]]. The [[window]] is defined in the [[windows]]-header.
                Like the [[graphics_environment]], creating a [[window]] requires a [[window_settings]], which can be used to adjust how the window functions. Creating a [[window]] also requires a [[graphics_environment]].
                Include the [[windows]] header, and write the following code to create a [[window]]:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::window_settings win_settings{
    .name = "Hello World",
};
CompWolf::window win(environment, win_settings);`}
            </CodeViewer>
            <FormattedText>
                If we run the code now, the window will close immediately.
                [[window]] has a method running(), which tells us if the window has not been closed. We can therefore wait until someone closes the window with a while loop:
            </FormattedText>
            <CodeViewer>
                {`while (win.running()) {}`}
            </CodeViewer>
            <FormattedText>
                The window will now stay open, but we cannot close it.
                We need to continually call environment.update() to be able to interact with the window.
                Therefore, to make the window stay open and be interactable, write:
            </FormattedText>
            <CodeViewer>
                {`while (win.running())
{
    environment.update();
}`}
            </CodeViewer>
            <FormattedText>
                Congratulations, you have now created a window.
            </FormattedText>
            <hr />
            <Link href="/tutorial/your-first-triangle">Click here for the next tutorial.</Link>
            <h2>Final code</h2>
            <CodeViewer>
                {`#include <graphics>
#include <windows>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    while (win.running())
    {
        environment.update();
    }
}`}
            </CodeViewer>
        </div>
    );
}
