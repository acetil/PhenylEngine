#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvOut;

layout(location = 0) out vec2 uv;
void main () {
    //gl_Position = camera * vec4((transform * position + absolutePos), 0, 1);
    //gl_Position = camera * vec4(transform * position + absolutePos, 0, 1);
    gl_Position = vec4(position, 0, 1);
    //gl_Position = vec4(position, 0, 1);
    uv = uvOut;
}