#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvOut;
out vec2 uv;
uniform mat4 camera;
void main () {
    gl_Position = camera * vec4(position, 0, 1);
    //gl_Position = vec4(position, 0, 1);
    uv = uvOut;
}