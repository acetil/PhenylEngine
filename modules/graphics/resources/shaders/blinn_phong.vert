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

out BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
    vec4 fragLightSpacePos;
} vsOut;

void main () {
    vec4 pos;
    pos = projection * view * model * vec4(position, 1.0);
    gl_Position = projection * view * model * vec4(position, 1.0);
    //gl_Position = vec4(pos.xyz / pos.w, 1.0);
    uv = texcoord_0;

    vsOut.fragPos = (model * vec4(position, 1.0)).xyz;
    //vsOut.fragPos = (view * model * vec4(position, 1.0)).xyz;
    vsOut.normal = normalize(((transpose(inverse(model))) * vec4(normal, 0.0f)).xyz);
    vsOut.viewPos = viewPos;

    vec4 lightSpacePos;
    vsOut.fragLightSpacePos = BPLight_lightSpace * model * vec4(position, 1.0);
    //vsOut.fragLightSpacePos = lightSpacePos.xyz / lightSpacePos.w;
}