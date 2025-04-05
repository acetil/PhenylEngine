#version 450

layout(location = 0) in vec3 uv;
layout(location = 1) in vec4 color_pass;

layout(location = 0) out vec4 color;

void main () {
    color = vec4(color_pass.rgb, color_pass.a);
}
