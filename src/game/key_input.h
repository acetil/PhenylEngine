#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"
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
    class KeyboardInput {
        public:
        virtual void setKey (int key, KeyboardFunction* function) = 0;
        virtual void setMouseButton (int button, MouseFunction* function) = 0;

        [[maybe_unused]] virtual void replaceKey (int after, int before) = 0;
        virtual void handleKeyPresses () = 0;
    };
    KeyboardInput* getKeyboardInput (graphics::Graphics* graphics);

    void setupKeyboardInputListeners (KeyboardInput* input, event::EventBus* bus);
}
#endif