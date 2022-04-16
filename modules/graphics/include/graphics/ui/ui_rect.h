#pragma once

#include "graphics/maths_headers.h"

namespace graphics {
    class UIRect {
    public:
        const glm::vec2 screenPos;
        const glm::vec2 size;
        const glm::vec4 borderColour;
        const glm::vec4 bgColour;
        const glm::vec4 details;

        UIRect (glm::vec2 _screenPos, glm::vec2 _size, glm::vec4 _borderColour, glm::vec4 _bgColour, glm::vec4 _details) : screenPos{_screenPos},
                size{_size}, borderColour{_borderColour}, bgColour{_bgColour}, details{_details} {}

    };
}