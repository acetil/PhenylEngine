#pragma once

#include "graphics/maths_headers.h"

namespace graphics::ui {

    /*enum class SizeAnchor {
        FIXED_SIZE,
        FILL,
        FILL_UP,
        FILL_DOWN,
        FILL_LEFT,
        FILL_RIGHT
    };*/

    enum class FloatAnchor {
        AUTO,
        FLOAT_START,
        FLOAT_END,
        FLOAT_CENTRE
    };


    class UIAnchor {
    public:
        const glm::vec2 minimumSize{0, 0};
        const glm::vec2 maximumSize{-1, -1};
        const glm::vec2 topLeftMargin{};
        const glm::vec2 bottomRightMargin{};

        const FloatAnchor horizFloatAnchor = FloatAnchor::AUTO;
        const FloatAnchor vertFloatAnchor = FloatAnchor::AUTO;

        //UIAnchor () = default;

        UIAnchor withMinimumSize (glm::vec2 _minimumSize) {
            return {_minimumSize, maximumSize, topLeftMargin, bottomRightMargin, horizFloatAnchor, vertFloatAnchor};
        }

        UIAnchor withMaximumSize (glm::vec2 _maximumSize) {
            return {.minimumSize = minimumSize, .maximumSize = _maximumSize, .topLeftMargin = topLeftMargin,
                    .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor};
        }

        UIAnchor withWidthRange (float minWidth, float maxWidth) {
            return {.minimumSize = {minWidth, minimumSize.y}, .maximumSize = {maxWidth, maximumSize.y}, .topLeftMargin = topLeftMargin,
                    .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor};
        }

        UIAnchor withHeightRange (float minHeight, float maxHeight) {
            return {.minimumSize = {minimumSize.x, minHeight}, .maximumSize = {maximumSize.x, maxHeight}, .topLeftMargin = topLeftMargin,
                    .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor};
        }

        UIAnchor withMargin (float upMargin, float leftMargin, float downMargin, float rightMargin) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize, .topLeftMargin = {leftMargin, upMargin}, .bottomRightMargin = {rightMargin, downMargin}, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor};
        }

        /*UIAnchor withHorizontalSizeAnchor (SizeAnchor horizontalSize) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }

        UIAnchor withVerticalSizeAnchor (SizeAnchor verticalSize) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin,  .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor, .fixedPos = fixedPos};
        }*/

        UIAnchor withHorizontalFloatAnchor (FloatAnchor horizontalFloat) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize,.topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizontalFloat,
                    .vertFloatAnchor = vertFloatAnchor};
        }

        UIAnchor withVerticalFloatAnchor (FloatAnchor verticalFloat) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = verticalFloat};
        }

        UIAnchor withFloatAnchor (FloatAnchor anchor) {
            return {.minimumSize = minimumSize, .maximumSize = maximumSize, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = anchor,
                    .vertFloatAnchor = anchor};
        }

        UIAnchor withFixedSize (glm::vec2 size) {
            return {.minimumSize = size, .maximumSize = size, .topLeftMargin = topLeftMargin, .bottomRightMargin = bottomRightMargin, .horizFloatAnchor = horizFloatAnchor,
                    .vertFloatAnchor = vertFloatAnchor};
        }
    };
}