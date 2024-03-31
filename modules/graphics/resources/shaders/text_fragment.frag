#version 330 core

in vec3 uv;
in vec3 color_pass;
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
    //float s = smoothstep(SMOOTH_MIN, SMOOTH_MAX, v);
    /*if (s < ALPHA_THRESHOLD) {
        color = vec4(color_pass * s, s);
    } else {
        color = vec4(color_pass * s, 1.0f);
    }*/
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //color = vec4(v, v, v, 1.0f);
    //color = texture(textureSampler, uv);
    //color = vec4(val, val, val, 1.0f);
    //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    color = vec4(color_pass * s, 1.0f);
}
