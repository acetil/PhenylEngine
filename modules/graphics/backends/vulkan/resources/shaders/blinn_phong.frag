#version 450

layout(std140, binding = 2) uniform Material {
    vec3 Material_color;
    //vec3 specular;
    vec3 Material_specular_color;
    float Material_ambient;
    float Material_specular;
    float Material_alpha;
};

layout(std140, set = 4, binding = 1) uniform BPLightUniform {
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

layout(location = 0) out vec4 color;

void main () {
    color = vec4(BPLight_color * Material_color, 1);
}
