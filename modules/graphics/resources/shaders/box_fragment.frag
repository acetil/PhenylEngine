#version 330 core

in vec2 rectPos;
in vec4 borderColour;
in vec4 bgColour;
in vec4 boxDetail; // width, height, radius, borderSize

out vec4 color;

//#define CORNER_RADIUS 0
#define ALIAS_SIZE 0.35
#define MIN_ALIAS_SIZE 0.6

void main() {
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
    //color = boxDetail;
    //color = bgColour;
}
