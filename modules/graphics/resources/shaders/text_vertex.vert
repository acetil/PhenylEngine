#version 430 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvOut;
layout(location = 2) in vec3 colorOut;

out vec2 uv;
out vec3 color_pass;

void main() {
    gl_Position = vec4(pos, 0, 1);
    uv = uvOut;
    color_pass = colorOut;
}
