#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 uvOut;
layout(location = 2) in vec4 colorOut;

layout(std140, binding = 0) uniform Uniform {
    vec2 screenSize;
};

layout(location = 0) out vec3 uv;
layout(location = 1) out vec4 color_pass;

void main() {
    vec2 glPos = vec2(pos.x - screenSize.x / 2.0f, pos.y - screenSize.y / 2.0f) / (screenSize / 2.0f);

    gl_Position = vec4(glPos, 0, 1);
    uv = uvOut;
    color_pass = colorOut;
}
