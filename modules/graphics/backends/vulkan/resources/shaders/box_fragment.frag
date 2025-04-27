#version 450

layout(location = 0) in vec2 rectPos;
layout(location = 1) in vec4 borderColour;
layout(location = 2) in vec4 bgColour;
layout(location = 3) in vec4 boxDetail;

layout(location = 0) out vec4 color;

#define ALIAS_SIZE 0.35
#define MIN_ALIAS_SIZE 0.6

void main () {
    vec2 disp = vec2(1 - abs(rectPos.x), 1 - abs(rectPos.y)) * boxDetail.xy / 2;
    float radius = boxDetail.z;
    float borderSize = boxDetail.a;
    float aliasSize = max(borderSize * ALIAS_SIZE, MIN_ALIAS_SIZE);
    if (disp.x > radius || disp.y > radius) {
        color = (disp.x < borderSize || disp.y < borderSize) ? borderColour : bgColour;
    } else {
        vec2 dist = vec2(radius, radius) - disp;
        if (dot(dist, dist) > (radius) * (radius)) {
            vec4 aliasColour = borderSize > 0 ? borderColour : bgColour;

            float aliasAmount = max(0.0f, (radius + aliasSize) * (radius + aliasSize) - dot(dist, dist)) / ((radius + aliasSize) * (radius + aliasSize) - radius * radius);

            color = aliasColour * aliasAmount;
        } else if (dot(dist, dist) > (radius - borderSize) * (radius - borderSize)){
            color = borderColour;
        } else {
            color = bgColour;
        }
    }
}

