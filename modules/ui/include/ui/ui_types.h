#pragma once
#include "event.h"
#include "graphics/maths_headers.h"

#include <functional>
#include <optional>

namespace phenyl::graphics {
class UIEvent;

using UIEventHandler = std::function<bool(const UIEvent&)>;

struct Modifier {
    glm::vec2 minSize = {0.0f, 0.0f};
    std::optional<float> maxWidth = std::nullopt;
    std::optional<float> maxHeight = std::nullopt;
    float padding = 0.0f;
    float weight = 0.0f;
    glm::vec2 offset = {0.0f, 0.0f};
    std::vector<UIEventHandler> handlers;

    [[nodiscard]] Modifier withSize (glm::vec2 size) const noexcept {
        Modifier copy = *this;
        copy.minSize = size;
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

    Modifier handle (std::function<bool(const UIEvent&)> handler) {
        handlers.emplace_back(std::move(handler));
        return std::move(*this);
    }

    template <typename T>
    Modifier handle (std::function<bool(const T&)> handler) {
        return handle([handler = std::move(handler)] (const UIEvent& event) {
            const auto* typedEvent = event.get<T>();
            return typedEvent ? handler(*typedEvent) : false;
        });
    }

    template <typename T>
    Modifier handle (std::function<void(const T&)> handler) {
        return handle([handler = std::move(handler)] (const T& event) {
            handler(event);
            return false;
        });
    }

    template <typename F>
    Modifier handle (F&& fn) {
        return handle(std::function{std::forward<F>(fn)});
    }
};

struct WidgetConstraints {
    std::optional<float> minWidth;
    std::optional<float> minHeight;
    glm::vec2 maxSize;
};

enum class LayoutArrangement {
    START,
    END,
    CENTER,
    SPACED
};

enum class LayoutAlignment {
    START,
    END,
    CENTER
};

enum class ColumnDirection {
    UP,
    DOWN
};

enum class RowDirection {
    LEFT,
    RIGHT
};
} // namespace phenyl::graphics
