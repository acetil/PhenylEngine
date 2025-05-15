#pragma once

#include <vector>

namespace phenyl::core {
class ButtonInputSource;

class ButtonInputBinding {
public:
    void addSource (const ButtonInputSource* source);
    void poll ();

    [[nodiscard]] bool state () const noexcept {
        return m_state;
    }

private:
    std::vector<const ButtonInputSource*> m_sources;
    bool m_state = false;
};

class InputAction {
public:
    InputAction ();
    explicit InputAction (const ButtonInputBinding* binding);

    explicit operator bool () const noexcept {
        return m_binding;
    }

    [[nodiscard]] bool value () const noexcept {
        return m_binding->state();
    }

private:
    const ButtonInputBinding* m_binding = nullptr;
};
} // namespace phenyl::core
