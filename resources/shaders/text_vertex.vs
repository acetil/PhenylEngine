#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvOut;

out vec2 uv;

uniform mat4 camera;

void main() {
    gl_Position = vec4(pos, 0, 1);
    uv = uvOut;
}
