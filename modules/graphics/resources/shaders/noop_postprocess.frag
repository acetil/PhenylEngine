#version 430

in vec2 uv;

uniform sampler2D frameBuffer;

out vec4 color;

void main() {
    color = texture(frameBuffer, uv);
}