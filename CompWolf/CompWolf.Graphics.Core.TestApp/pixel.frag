#version 450

layout(location = 0) in vec2 fragUV;

layout(binding = 4) uniform sampler2D image;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(image, fragUV);
}