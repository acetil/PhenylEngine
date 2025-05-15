#pragma once

namespace phenyl::graphics {
enum class CanvasFill {
    FILLED,
    OUTLINE
};

struct CanvasStyle {
    CanvasFill fill = CanvasFill::FILLED;
    float outlineSize = 1.0f;
    glm::vec4 colour = glm::vec4{1.0f, 1.0f, 10.0f, 1.0f};

    bool useAA = false;
    unsigned int quanta = 20;
};
} // namespace phenyl::graphics
