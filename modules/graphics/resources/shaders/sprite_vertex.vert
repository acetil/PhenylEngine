#version 430

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvOut;
//layout(location = 2) in vec2 absolutePos;
//layout(location = 3) in mat2 transform;
out vec2 uv;
//uniform mat4 camera;
layout(std140) uniform Camera {
    mat4 camera;
};
void main () {
    //gl_Position = camera * vec4((transform * position + absolutePos), 0, 1);
    //gl_Position = camera * vec4(transform * position + absolutePos, 0, 1);
    gl_Position = camera * vec4(position, 0, 1);
    //gl_Position = vec4(position, 0, 1);
    uv = uvOut;
}