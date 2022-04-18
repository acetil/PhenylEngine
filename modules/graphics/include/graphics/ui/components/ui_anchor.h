#pragma once

#include "graphics/maths_headers.h"

namespace graphics::ui {

    enum class SizeAnchor {
        FIXED_SIZE,
        FILL,
        FILL_UP,
        FILL_DOWN,
        FILL_LEFT,
        FILL_RIGHT
    };

    enum class FloatAnchor {
        FREE,
        FIXED,
        FLOAT_CENTER,
        FLOAT_LEFT,
        FLOAT_RIGHT,
        FLOAT_UP,
        FLOAT_DOWN
    };


    class UIAnchor {
    public:
        const glm::vec2 minimumSize{};
        const glm::vec2 topLeftMargin{};
        const glm::vec2 bottomRightMargin{};

        const SizeAnchor horizSizeAnchor = SizeAnchor::FIXED_SIZE;
        const SizeAnchor vertSizeAnchor = SizeAnchor::FIXED_SIZE;

        const FloatAnchor horizFloatAnchor = FloatAnchor::FREE;
        const FloatAnchor vertFloatAnchor = FloatAnchor::FREE;
        const glm::vec2 fixedPos{};

        //UIAnchor () = default;

        UIAnchor withMinimumSize (glm::vec2 _minimumSize) {
            return {_minimumSize, topLeftMargin, bottomRightMargin, horizSizeAnchor, vertSizeAnchor, horizFloatAnchor, vertFloatAnchor, fixedPos};
        }

        UIAnchor withMargin (float upMargin, float leftMargin, float downMargin, float rightMargin) {
            return {.minimumSize = minimumSize, .topLeftMargin = {leftMargin, upMargin}, .bottomRightMargin = {rightMargin, downMargin}, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withHorizontalSizeAnchor (SizeAnchor horizontalSize) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizontalSize,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withVerticalSizeAnchor (SizeAnchor verticalSize) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = verticalSize, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withHorizontalFloatAnchor (FloatAnchor horizontalFloat) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = horizontalFloat,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withVerticalFloatAnchor (FloatAnchor verticalFloat) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withFixedSize () {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = SizeAnchor::FIXED_SIZE,
                    .vertSizeAnchor = SizeAnchor::FIXED_SIZE, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withFixedHorizontalFloat (float horizontalPos) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = FloatAnchor::FIXED,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = {horizontalPos, fixedPos.y}};
        }

        UIAnchor withFixedVerticalFloat (float verticalPos) {
            return {.minimumSize = minimumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizSizeAnchor = horizSizeAnchor,
                    .vertSizeAnchor = vertSizeAnchor, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = FloatAnchor::FIXED, .fixedPos = {fixedPos.x, verticalPos}};
        }
    };
}