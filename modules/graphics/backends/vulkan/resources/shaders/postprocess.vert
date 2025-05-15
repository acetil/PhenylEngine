#version 450

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 uv;

void main() {
    gl_Position = vec4(position, 0, 1);

    uv = vec2((position.x + 1.0) / 2.0, (position.y + 1.0) / 2.0);
}
