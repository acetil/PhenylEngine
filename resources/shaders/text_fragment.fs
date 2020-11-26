#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D textureSampler;

#define SMOOTH_MIN 0.47f
#define SMOOTH_MAX 0.53f

#define THRESHOLD 0.5f

void main() {
    float val = texture(textureSampler, uv).r;
    float s = smoothstep(SMOOTH_MIN, SMOOTH_MAX, val);
    color = vec4(1.0f, 1.0f, 1.0f, s);
    //color = texture(textureSampler, uv);
    //color = vec4(val, val, val, 1.0f);
    //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
