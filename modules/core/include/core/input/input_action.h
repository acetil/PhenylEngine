#pragma once

#include <vector>

namespace phenyl::core {
    class ButtonInputSource;

    class ButtonInputBinding {
    private:
        std::vector<const ButtonInputSource*> m_sources;
        bool m_state = false;
    public:
        void addSource (const ButtonInputSource* source);
        void poll ();

        [[nodiscard]] bool state () const noexcept {
            return m_state;
        }
    };


    class InputAction {
    private:
        const ButtonInputBinding* m_binding = nullptr;

    public:
        InputAction ();
        explicit InputAction (const ButtonInputBinding* binding);

        explicit operator bool () const noexcept {
            return m_binding;
        }

        [[nodiscard]] bool value () const noexcept {
            return m_binding->state();
        }
    };
}
