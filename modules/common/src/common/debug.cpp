#include "common/debug.h"
#include "graphics/debug_graphics.h"

using namespace phenyl;

void common::debugWorldRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour) {
    debugWorldRect(topLeft, glm::vec2{bottomRight.x, topLeft.y}, glm::vec2{topLeft.x, bottomRight.y}, bottomRight, colour, outlineColour);
}

void common::debugWorldRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
    graphics::debugWorldRect(pos1, pos2, pos3, pos4, colour, outlineColour);
}

void common::debugWorldRectOutline (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 outlineColour) {
    debugWorldRect(pos1, pos2, pos3, pos4, glm::vec4{}, outlineColour);
}

void common::debugWorldRectOutline (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 outlineColour) {
    //graphics::debugWorldRect(topLeft, bottomRight, {0, 0, 0, 0}, outlineColour);
    debugWorldRect(topLeft, bottomRight, glm::vec4{0, 0, 0, 0}, outlineColour);
}

void common::debugRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour) {
    debugRect(topLeft, glm::vec2{bottomRight.x, topLeft.y}, glm::vec2{topLeft.x, bottomRight.y}, bottomRight, colour, outlineColour);
}

void common::debugRectOutline (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 outlineColour) {
    debugRect(topLeft, bottomRight, glm::vec4{}, outlineColour);
}

void common::debugRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
    graphics::debugScreenRect(pos1, pos2, pos3, pos4, colour, outlineColour);
}

void common::debugRectOutline (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 outlineColour) {
    debugRect(pos1, pos2, pos3, pos4, glm::vec4{}, outlineColour);
}

void common::debugWorldLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    graphics::debugWorldLine(start, end, colour);
}

void common::debugLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    graphics::debugScreenLine(start, end, colour);
}