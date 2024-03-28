#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor1;

layout(binding = 0) uniform Transform {
    vec2 position;
} transform;
layout(binding = 1) uniform Transform2 {
    float scaler;
} transform2;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition * transform2.scaler + transform.position, 0.0, 1.0);
    fragColor = inColor1;
}