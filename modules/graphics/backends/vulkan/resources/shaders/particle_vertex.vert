#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 colourIn;

layout(std140, binding = 0) uniform Camera {
    mat4 camera;
};

layout(location = 0) out vec4 fragColour;

void main () {
    gl_Position = camera * vec4(pos, 0, 1);
    fragColour = colourIn;
}
