#pragma once

#include <optional>

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    class Canvas;

    struct Modifier {
        glm::vec2 minSize = {0.0f, 0.0f};
        std::optional<float> maxWidth = std::nullopt;
        std::optional<float> maxHeight = std::nullopt;
        float padding = 0.0f;
        float weight = 0.0f;
        glm::vec2 offset = {0.0f, 0.0f};

        [[nodiscard]] Modifier withSize (glm::vec2 size) const noexcept {
            Modifier copy = *this;
            copy.minSize = minSize;
            copy.maxWidth = size.x;
            copy.maxHeight = size.y;

            return copy;
        }

        [[nodiscard]] Modifier withSize (glm::vec2 minSize, glm::vec2 maxSize) const noexcept {
            Modifier copy = *this;
            copy.minSize = minSize;
            copy.maxWidth = maxSize.x;
            copy.maxHeight = maxSize.y;

            return copy;
        }

        [[nodiscard]] Modifier withPadding (float padding) const noexcept {
            Modifier copy = *this;
            copy.padding = padding;

            return copy;
        }

        [[nodiscard]] Modifier withWeight (float weight) const noexcept {
            Modifier copy = *this;
            copy.weight = weight;

            return copy;
        }

        [[nodiscard]] Modifier withOffset (glm::vec2 offset) const noexcept {
            Modifier copy = *this;
            copy.offset = offset;

            return copy;
        }
    };

    struct WidgetConstraints {
        std::optional<float> minWidth;
        std::optional<float> minHeight;
        glm::vec2 maxSize;
    };


    class Widget {
    private:
        Modifier widgetModifier;
        Widget* parentWidget = nullptr;
        glm::vec2 widgetSize = {0, 0};
    protected:
        virtual void queueChildDestroy (Widget* child);
        void setModifier (const Modifier& modifier);
        void setDimensions (glm::vec2 newDims);
    public:
        explicit Widget (const Modifier& modifier = Modifier{});
        virtual ~Widget ();

        virtual void measure (const WidgetConstraints& constraints);
        virtual void render (Canvas& canvas) = 0;

        virtual void update ();
        void queueDestroy ();

        [[nodiscard]] Widget* parent () const noexcept {
            return parentWidget;
        }
        void setParent (Widget* parent);

        [[nodiscard]] const Modifier& modifier () const noexcept {
            return widgetModifier;
        }

        glm::vec2 dimensions () const noexcept {
            return widgetSize;
        }
    };
}
