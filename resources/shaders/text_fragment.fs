#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D textureSampler;

#define SMOOTH_MIN 0.49f
#define SMOOTH_MAX 0.51f

#define THRESHOLD 0.5f

void main() {
    float v = texture(textureSampler, uv).r;
    float s = smoothstep(SMOOTH_MIN, SMOOTH_MAX, v);
    color = vec4(1.0f, 1.0f, 1.0f, s);
    //color = vec4(v, v, v, 1.0f);
    //color = texture(textureSampler, uv);
    //color = vec4(val, val, val, 1.0f);
    //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
