#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 rectPosIn;
layout(location = 2) in vec4 borderColourIn;
layout(location = 3) in vec4 bgColourIn;
layout(location = 4) in vec4 boxDetailIn;

out vec2 rectPos;
out vec4 borderColour;
out vec4 bgColour;
out vec4 boxDetail;

uniform vec2 screenSize;

void main() {
    gl_Position = vec4(pos.x / screenSize.x * 2.0f - 1.0f, pos.y / screenSize.y * -2.0f + 1.0f, 0, 1);

    rectPos = rectPosIn;
    borderColour = borderColourIn;
    bgColour = bgColourIn;
    boxDetail = boxDetailIn;
}
