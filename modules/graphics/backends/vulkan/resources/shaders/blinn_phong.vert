#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord_0;

layout (location = 3) in mat4 model;

void main () {
    gl_Position = model * vec4(position, 1.0);
}