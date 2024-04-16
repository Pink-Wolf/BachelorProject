#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(binding = 9) uniform Transform {
    vec2 position;
} transform;

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = vec4(inPosition + transform.position, 0.0, 1.0);
    fragUV = inUV;
}