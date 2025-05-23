#version 450

layout(location = 0) in vec2 uv;

layout(binding = 0) uniform sampler2D frameBuffer;

layout(location = 0) out vec4 color;

void main() {
    color = texture(frameBuffer, uv);
}
