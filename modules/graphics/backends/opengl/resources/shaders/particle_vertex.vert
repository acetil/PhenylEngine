#version 430

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 colourIn;

out vec4 fragColour;

//uniform mat4 camera;
layout(std140) uniform Camera {
    mat4 camera;
};

void main () {
    gl_Position = camera * vec4(pos, 0, 1);
    fragColour = colourIn;
}
