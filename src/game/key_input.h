#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"
#include "graphics/graphics_new.h"
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
    class KeyboardInput {
        public:
        virtual void setKey (int key, KeyboardFunction* function) = 0;
        virtual void replaceKey (int after, int before) = 0;
        virtual void handleKeyPresses () = 0;
    };
    KeyboardInput* getKeyboardInput (graphics::GraphicsNew* graphics);
};
#endif