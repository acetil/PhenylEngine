#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvOut;

layout(location = 0) out vec2 uv;

layout(std140, binding = 0) uniform Camera {
    mat4 camera;
};
void main () {
    gl_Position = camera * vec4(position, 0, 1);
    uv = uvOut;
}