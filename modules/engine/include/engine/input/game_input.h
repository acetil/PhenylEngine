#pragma once

#include <memory>
#include <vector>

#include "common/input/remappable_input.h"
#include "graphics/maths_headers.h"

namespace graphics {
    class Renderer;
}

namespace engine::detail {
    class Engine;
}

namespace game {
    class GameInput {
    private:
        graphics::Renderer* renderer; // TODO: window class
        std::unique_ptr<common::RemappableInput> inputSource;
        void setRenderer (graphics::Renderer* renderer);
        friend class engine::detail::Engine;
    public:
        GameInput ();
        common::InputAction mapInput (const std::string& actionName, const std::string& inputName);
        common::InputAction getInput (const std::string& actionName);

        bool isDown (const common::InputAction& action);
        glm::vec2 cursorPos () const;
        glm::vec2 screenSize () const; // TODO: move?

        void poll ();

        void addInputSources (const std::vector<std::shared_ptr<common::InputSource>>& sources);
    };
}