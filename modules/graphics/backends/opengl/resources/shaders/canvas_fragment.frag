#version 330 core

in vec3 uv;
in vec4 color_pass;
out vec4 color;

uniform sampler2DArray textureSampler;


void main() {
    float s = texture(textureSampler, uv).r;
    color = vec4(color_pass.rgb, color_pass.a * s);
}
