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
    class KeyboardInput {
        private:
        GLFWwindow* window;
        std::map<int, KeyboardFunction*> bindingMap; 
        public:
        KeyboardInput (graphics::Graphics* graphics);
        ~KeyboardInput ();
        void setKey (int key, KeyboardFunction* function);
        void replaceKey (int after, int before);
        void handleKeyPresses ();
    };
};
#endif