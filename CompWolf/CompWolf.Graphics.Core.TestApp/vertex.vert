#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(binding = 0) uniform Transform {
    vec2 position;
} transform;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition + transform.position, 0.0, 1.0);
    fragColor = inColor;
}