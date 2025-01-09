#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord_0;

layout (location = 3) in mat4 model;

out vec2 uv;

layout(std140) uniform GlobalUniform {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

out BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
} vsOut;

void main () {
    vec4 pos;
    pos = projection * view * model * vec4(position, 1.0);
    gl_Position = projection * view * model * vec4(position, 1.0);
    uv = texcoord_0;

    vsOut.fragPos = (model * vec4(position, 1.0)).xyz;
    vsOut.normal = normalize(((transpose(inverse(model))) * vec4(normal, 0.0f)).xyz);
    vsOut.viewPos = viewPos;
}