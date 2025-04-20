#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord_0;

layout (location = 3) in mat4 model;

layout(std140, binding = 0) uniform GlobalUniform {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

layout(std140, binding = 1) uniform BPLightUniform {
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

layout(location = 0) out BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
    vec4 fragLightSpacePos;
} vsOut;

void main () {
    gl_Position = projection * view * model * vec4(position, 1.0);
}