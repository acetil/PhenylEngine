#version 430 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 uvOut;
layout(location = 2) in vec3 colorOut;

layout(std140) uniform Uniform {
    vec2 screenSize;
};

out vec3 uv;
out vec3 color_pass;

void main() {
    vec2 glPos = vec2(pos.x - screenSize.x / 2.0f, -pos.y + screenSize.y / 2.0f) / (screenSize / 2.0f);

    gl_Position = vec4(glPos, 0, 1);
    uv = uvOut;
    color_pass = colorOut;
}
