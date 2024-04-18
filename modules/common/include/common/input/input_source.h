#pragma once

namespace phenyl::common {
    class ButtonInputSource {
    private:
        bool currState = false;
    public:
        [[nodiscard]] bool state () const noexcept {
            return currState;
        }

        void setState (bool newState) {
            currState = newState;
        }
    };
}