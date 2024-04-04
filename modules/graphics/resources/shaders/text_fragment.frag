#version 330 core

in vec3 uv;
in vec4 color_pass;
out vec4 color;

uniform sampler2DArray textureSampler;

#define SMOOTH_MIN 0.45f
#define SMOOTH_MAX 0.53f

#define THRESHOLD 0.5f

#define ALPHA_THRESHOLD 0.30f

/*float bilinealSample (vec2 uv) {

}*/


void main() {
    float s = texture(textureSampler, uv).r;
    color = vec4(color_pass.rgb * s, color_pass.a);
}
