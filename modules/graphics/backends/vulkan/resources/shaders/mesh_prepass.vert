#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in mat4 model;

layout(std140, binding = 0) uniform GlobalUniform {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};


void main () {
    mat4 fixY = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, -1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(0, 0, 0, 1)
    );

    gl_Position = projection * fixY * view * model * vec4(position, 1.0);
}
