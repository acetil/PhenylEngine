#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"

#include "util/smart_help.h"

#include <vector>
#include <map>
#include <utility>

#ifndef KEY_INPUT_H
#define KEY_INPUT_H
namespace game {
    class KeyboardFunction {
        public:
        virtual void operator() (int action) = 0;
    };
    class MouseFunction {
    public:
        virtual void operator() (int action, glm::vec2 screenPos, glm::vec2 worldPos) = 0;
    };
class KeyboardInput : public util::SmartHelper<KeyboardInput> {
        public:
        virtual void setKey (int key, KeyboardFunction* function) = 0;
        virtual void setMouseButton (int button, MouseFunction* function) = 0;

        [[maybe_unused]] virtual void replaceKey (int after, int before) = 0;
        virtual void handleKeyPresses () = 0;
    };
    KeyboardInput::SharedPtr getKeyboardInput (const graphics::Graphics::SharedPtr& graphics);

    void setupKeyboardInputListeners (const KeyboardInput::SharedPtr& input, const event::EventBus::SharedPtr& bus);
}
#endif