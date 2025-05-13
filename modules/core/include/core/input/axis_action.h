#pragma once

#include "graphics/maths_headers.h"
#include "input_source.h"

#include <vector>

namespace phenyl::core {
class ButtonInputSource;
class Axis2DInputSource;

class Axis2DBinding {
public:
    explicit Axis2DBinding (bool normalised);
    void addButtonSource (const ButtonInputSource* source, glm::vec2 sourceVec);
    void addAxisSource (const Axis2DInputSource* source);
    void poll ();

    [[nodiscard]] glm::vec2 state () const noexcept {
        return m_state;
    }

private:
    std::vector<std::pair<const ButtonInputSource*, glm::vec2>> m_buttonSources;
    std::vector<const Axis2DInputSource*> m_axisSources;
    glm::vec2 m_state{};
    bool m_normalised;
};

class Axis2DInput {
public:
    Axis2DInput () = default;

    explicit Axis2DInput (const Axis2DBinding* binding) : m_binding{binding} {}

    explicit operator bool () const noexcept {
        return m_binding;
    }

    [[nodiscard]] glm::vec2 value () const noexcept {
        return m_binding->state();
    }

private:
    const Axis2DBinding* m_binding = nullptr;
};
} // namespace phenyl::core
