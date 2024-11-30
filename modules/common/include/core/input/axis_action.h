#pragma once

#include <vector>

#include "input_source.h"
#include "graphics/maths_headers.h"

namespace phenyl::core {
    class ButtonInputSource;
    class Axis2DInputSource;

    class Axis2DBinding {
    private:
        std::vector<std::pair<const ButtonInputSource*, glm::vec2>> buttonSources;
        std::vector<const Axis2DInputSource*> axisSources;
        glm::vec2 currState{};
        bool normalised;
    public:
        explicit Axis2DBinding (bool normalised);
        void addButtonSource (const ButtonInputSource* source, glm::vec2 sourceVec);
        void addAxisSource (const Axis2DInputSource* source);
        void poll ();

        [[nodiscard]] glm::vec2 state () const noexcept {
            return currState;
        }
    };

    class Axis2DInput {
    private:
        const Axis2DBinding* binding = nullptr;
    public:
        Axis2DInput () = default;
        explicit Axis2DInput (const Axis2DBinding* binding) : binding{binding} {}

        explicit operator bool () const noexcept {
            return binding;
        }

        [[nodiscard]] glm::vec2 value () const noexcept {
            return binding->state();
        }
    };
}
