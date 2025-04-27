#version 450

layout(location = 0) in vec3 uv;
layout(location = 1) in vec4 color_pass;

layout(binding = 1) uniform sampler2DArray textureSampler;

layout(location = 0) out vec4 color;

void main () {
    float s = texture(textureSampler, uv).r;
    color = vec4(color_pass.rgb, color_pass.a * s);
    //color = vec4(color_pass.rgb, 1) * s;
}
