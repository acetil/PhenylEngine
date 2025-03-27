#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in mat4 model;

layout(std140) uniform GlobalUniform {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};


void main () {
    gl_Position = projection * view * model * vec4(position, 1.0);
}