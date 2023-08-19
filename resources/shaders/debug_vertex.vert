#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colourOut;

out vec4 colour;

uniform mat4 camera;
uniform mat4 screenTransform;

void main () {
    float z = position.z;

    gl_Position = ((1 - z) * screenTransform + z * camera) * vec4(position.xy, 0, 1);
    colour = colourOut;
}