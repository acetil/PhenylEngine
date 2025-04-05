#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colourOut;

layout(location = 0) out vec4 colour;

void main () {
    float z = position.z;

    gl_Position = vec4(position.xy, 0, 1);
    colour = colourOut;
}