#pragma once

#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"

#include "util/smart_help.h"

#include <vector>
#include <map>
#include <utility>

namespace game {
    class KeyboardFunction {
        public:
        virtual void operator() (int action) = 0;
        virtual ~KeyboardFunction() = default;
    };
    class MouseFunction {
    public:
        virtual void operator() (int action, glm::vec2 windowPos, glm::vec2 screenPos) = 0;
        virtual ~MouseFunction() = default;
    };
class KeyboardInput : public util::SmartHelper<KeyboardInput> {
        public:
        virtual void setKey (int key, std::unique_ptr<KeyboardFunction> function) = 0;
        virtual void setMouseButton (int button, std::unique_ptr<MouseFunction> function) = 0;

        [[maybe_unused]] virtual void replaceKey (int after, int before) = 0;
        virtual void handleKeyPresses () = 0;
    };
    KeyboardInput::SharedPtr getKeyboardInput (const graphics::Graphics::SharedPtr& graphics);

    void setupKeyboardInputListeners (const KeyboardInput::SharedPtr& input, const event::EventBus::SharedPtr& bus);
}
