#pragma once

#include <memory>
#include <vector>

#include "common/input/remappable_input.h"
#include "graphics/maths_headers.h"
#include "runtime/plugin.h"

namespace phenyl::graphics {
    class Renderer;
}

namespace phenyl::engine::detail {
    class Engine;
}

namespace phenyl::game {
    class GameInput : public runtime::IResource {
    private:
        graphics::Renderer* renderer; // TODO: window class
        std::unique_ptr<common::RemappableInput> inputSource;
        void setRenderer (graphics::Renderer* renderer);
        friend class engine::detail::Engine;
        friend class GameInputPlugin;
    public:
        GameInput ();

        std::string_view getName() const noexcept override;

        common::InputAction mapInput (const std::string& actionName, const std::string& inputName);
        common::InputAction getInput (const std::string& actionName);

        bool isDown (const common::InputAction& action);
        glm::vec2 cursorPos () const;
        glm::vec2 screenSize () const; // TODO: move?

        void poll ();

        void addInputSources (const std::vector<std::shared_ptr<common::InputSource>>& sources);
    };

    class GameInputPlugin : public runtime::IPlugin {
    private:
        GameInput input;
    public:
        [[nodiscard]] std::string_view getName() const noexcept override;

        void init (runtime::PhenylRuntime& runtime) override;
        void frameBegin (runtime::PhenylRuntime& runtime) override;
    };
}
