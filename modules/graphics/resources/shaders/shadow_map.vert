#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in mat4 model;

layout(std140) uniform BPLightUniform {
    mat4 BPLight_lightSpace;
    vec3 BPLight_pos;
    vec3 BPLight_dir;
    vec3 BPLight_color;
    vec3 BPLight_ambient_color;
    float BPLight_brightness;
    float BPLight_outer;
    float BPLight_inner;
    int BPLight_type;
    bool BPLight_castShadows;
};

void main () {
    gl_Position = BPLight_lightSpace * model * vec4(position, 1.0);
}