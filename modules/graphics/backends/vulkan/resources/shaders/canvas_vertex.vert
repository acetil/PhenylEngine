#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 uvOut;
layout(location = 2) in vec4 colorOut;

layout(location = 0) out vec3 uv;
layout(location = 1) out vec4 color_pass;

void main() {
    vec2 glPos = pos;

    gl_Position = vec4(glPos, 0, 1);
    uv = uvOut;
    color_pass = colorOut;
}
