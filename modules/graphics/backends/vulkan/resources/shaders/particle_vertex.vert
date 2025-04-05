#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 colourIn;

layout(location = 0) out vec4 fragColour;

void main () {
    gl_Position = vec4(pos, 0, 1);
    fragColour = colourIn;
}