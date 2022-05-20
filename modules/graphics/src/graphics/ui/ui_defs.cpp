#include "graphics/ui/ui_defs.h"

using namespace graphics::ui;

glm::vec2 graphics::ui::getAxisVec (Axis axis) {
    switch (axis) {
        case Axis::LEFT:
            return {-1, 0};
        case Axis::RIGHT:
            return {1, 0};
        case Axis::UP:
            return {0, -1};
        case Axis::DOWN:
            return {0, 1};
    }
    return {0, 0};
}