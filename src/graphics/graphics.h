#include "graphics_headers.h"
#include <vector>
#ifndef GRAPHICS_H
    #define GRAPHICS_H
    namespace graphics {
        class Graphics {
            private:
            GLFWwindow* window;
            std::vector<GLuint> shaders;
        };
    }
#endif