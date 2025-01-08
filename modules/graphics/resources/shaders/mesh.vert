#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvOut;

layout (location = 3) in mat4 model;

out vec2 uv;
out vec4 colorOut;

layout(std140) uniform GlobalUniform {
    mat4 view;
    mat4 projection;
};

void main () {
    vec4 pos;
    pos = projection * view * model * vec4(position, 1.0);
    gl_Position = projection * view * model * vec4(position, 1.0);
    uv = uvOut;

    //colorOut = vec4(1 * max(1 - pos.z, 0), 0, 0, 1);
    //colorOut = vec4(1, 0, 0, 1);
    colorOut = vec4(abs(normal), 1);
}