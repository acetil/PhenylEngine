#version 330 core
in vec2 uv;
out vec4 color;
uniform sampler2D textureSampler;
void main () {
    color = texture(textureSampler, uv);
    //color.xyz = vec3(1,0,0);
}