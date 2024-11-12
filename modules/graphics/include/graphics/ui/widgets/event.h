#pragma once

#include <any>

#include "graphics/maths_headers.h"
#include "util/meta.h"

namespace phenyl::graphics {
    class UIEvent {
    private:
        std::any event;
        std::size_t eventType;
    public:
        template <typename T> requires (!std::same_as<T, UIEvent>)
        explicit UIEvent (T&& event) : event{std::forward<T>(event)}, eventType{meta::type_index<T>()} {}

        UIEvent (const UIEvent&) = delete;
        UIEvent (UIEvent&&) = default;

        UIEvent& operator= (const UIEvent&) = delete;
        UIEvent& operator= (UIEvent&&) = default;

        template <typename T>
        const T* get () const noexcept {
            return std::any_cast<const T>(&event);
        }

        template <typename T>
        [[nodiscard]] bool is () const noexcept {
            return get<T>();
        }

        std::size_t type () const noexcept {
            return eventType;
        }
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
}