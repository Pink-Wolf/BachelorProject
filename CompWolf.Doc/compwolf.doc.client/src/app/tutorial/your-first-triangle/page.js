import { CodeViewer, SimpleReference } from "@/lib/CodeComponents";
import FormattedText from "@/lib/FormattedText";
import Link from "next/link";

export default function Home() {
    return (
        <div>
            <h1>Your First Triangle</h1>
            This tutorial builds ontop of <Link href={"/tutorial/your-first-window"}>your-first-window</Link>; it is recommended to first complete that tutorial.
            <FormattedText>
                This tutorial will go over how to make a triangle be displayed on your window.
            </FormattedText>
            <h2>material</h2>
            <FormattedText>
                To be able to display anything, we must first create a [[draw_material]] which defines how to draw the thing.

                The material requires some [[shader]]s, which is code for the gpu. Your project includes 2 files called input_shader.spv and pixel_shader.spv. These contains the shader-code.
                Creating a [[shader]] requires a [[graphics_environment]], as well as its code. The code can be retrieved from a file with the function [[shader_code_from_file]].
                The [[shader]]s are defined in the [[shaders]]-header. Include the header, and write the following code to create the pixel_shader:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::shader<> pixel_shader(environment, CompWolf::shader_code_from_file("pixel_shader.spv"));`}
            </CodeViewer>
            <FormattedText>
                The input shader is a bit more complex than our pixel shader. It is not just a [[shader]], but an [[input_shader]]. It takes a float2 (2 floats) as its input, which lets us tell it the triangle's vertices.
                We must tell our program about this input, by passing float2 as a template parameter. Write the following to create the input_shader:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::input_shader<CompWolf::float2> input_shader(environment, CompWolf::shader_code_from_file("input_shader.spv"));`}
            </CodeViewer>
            <FormattedText>
                We can now create our [[draw_material]] with these shaders; include its header [[drawables]], and write the following code:
            </FormattedText>
            <CodeViewer>{`CompWolf::draw_material material(input_shader, pixel_shader);`}</CodeViewer>
            <h2>buffer</h2>
            <FormattedText>
                To be able to draw our triangle, we must first create a buffer with the vertices of our triangle.
                Since the vertices are the inputs for an [[input_shader]], we need to use a [[gpu_input_buffer]].
                To create a buffer, we must pass along its data, but also the gpu it is on. Since we want to draw the triangle on our window, we can get the right gpu with win.gpu().
                Write the following code to create a [[gpu_input_buffer]] with the vertices of our triangle:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::gpu_input_buffer<CompWolf::float2> vertices(win.gpu(), {
    {-.1, .1 },
    {0, -.1 },
    {.1, .1 },
});`}
            </CodeViewer>
            <FormattedText>
                To be able to draw our triangle, we must also create a [[gpu_index_buffer]]. This tells the triangle how to read the inputs.
                For a simple triangle, simply write the following code:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::gpu_index_buffer indices(win.gpu(), { 0, 1, 2 });`}
            </CodeViewer>
            <h2>drawable</h2>
            <FormattedText>
                With our material and buffers ready, we can finally create our triangle.
                Something that is drawn on a window is a [[drawable]].
                To create a [[drawable]], we must pass along the window, material, indices, and input/vertices.
                Write the following code to create our triangle:
            </FormattedText>
            <CodeViewer>
                {`CompWolf::drawable triangle(win, material, indices, vertices);`}
            </CodeViewer>
            <FormattedText>
                If we run the program now, we will not see any triangle. This is because we need to tell the window to update what it displays on screen with win.update_image().
                In the while loop, write:
            </FormattedText>
            <CodeViewer>
                {`win.update_image();`}
            </CodeViewer>
            <FormattedText>
                Congratulations, you have now created a triangle on your window.
            </FormattedText>
            <h2>Final code</h2>
            <CodeViewer>
                {`#include <graphics>
#include <windows>
#include <shaders>
#include <gpu_buffers>
#include <drawables>

CompWolf::graphics_environment environment;

int main()
{
    CompWolf::graphics_environment_settings environment_settings;

    environment = CompWolf::graphics_environment(environment_settings);

    CompWolf::window_settings win_settings{
        .name = "Hello World",
    };
    CompWolf::window win(environment, win_settings);

    CompWolf::shader<> pixel_shader(environment, CompWolf::shader_code_from_file("frag.spv"));
    CompWolf::input_shader<CompWolf::float2> input_shader(environment, CompWolf::shader_code_from_file("vertex.spv"));
    CompWolf::draw_material material(input_shader, pixel_shader);
    
    CompWolf::gpu_input_buffer<CompWolf::float2> vertices(win.gpu(), {
        {-.1, .1 },
        {0, -.1 },
        {.1, .1 },
    });
    CompWolf::gpu_index_buffer indices(win.gpu(), { 0, 1, 2 });

    CompWolf::drawable triangle(win, material, indices, vertices);


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
