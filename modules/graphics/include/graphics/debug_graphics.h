#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    //void debugWorldRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour);
    void debugWorldRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour);

    //void debugScreenRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour);
    void debugScreenRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour);
    void debugWorldLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);
    void debugScreenLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);
}