#pragma once

#include <vector>

namespace phenyl::common {
    class ButtonInputSource;

    class ButtonInputBinding {
    private:
        std::vector<const ButtonInputSource*> sources;
        bool currState = false;
    public:
        void addSource (const ButtonInputSource* source);
        void poll ();

        [[nodiscard]] bool state () const noexcept {
            return currState;
        }
    };


    class InputAction {
    private:
        const ButtonInputBinding* binding = nullptr;

    public:
        InputAction ();
        explicit InputAction (const ButtonInputBinding* binding);

        explicit operator bool () const noexcept {
            return binding;
        }

        [[nodiscard]] bool value () const noexcept {
            return binding->state();
        }
    };
}
