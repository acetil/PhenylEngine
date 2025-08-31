#pragma once

#include "graphics/maths_headers.h"
#include "util/type_index.h"

#include <any>

namespace phenyl::graphics {
class UIEvent {
public:
    template <typename T>
    requires (!std::same_as<T, UIEvent>)
    explicit UIEvent(T&& event) : m_event{std::forward<T>(event)}, m_type{meta::TypeIndex::Get<T>()} {}

    UIEvent (const UIEvent&) = delete;
    UIEvent (UIEvent&&) = default;

    UIEvent& operator= (const UIEvent&) = delete;
    UIEvent& operator= (UIEvent&&) = default;

    template <typename T>
    const T* get () const noexcept {
        return std::any_cast<const T>(&m_event);
    }

    template <typename T>
    [[nodiscard]] bool is () const noexcept {
        return get<T>();
    }

    meta::TypeIndex type () const noexcept {
        return m_type;
    }

private:
    std::any m_event;
    meta::TypeIndex m_type;
};

struct MouseEnterEvent {
    glm::vec2 pos{};
};

struct MouseMoveEvent {
    glm::vec2 oldPos{};
    glm::vec2 newPos{};
};

struct MouseExitEvent {};

struct MousePressEvent {};

struct MouseReleaseEvent {};
} // namespace phenyl::graphics
