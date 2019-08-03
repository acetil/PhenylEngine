#include "graphics_headers.h"
#include "shaders/shaders.h"
#include <vector>
#include <unordered_map>
#ifndef GRAPHICS_H
    #define GRAPHICS_H
    namespace graphics {
        class Graphics {
            private:
                GLFWwindow* window;
                std::unordered_map<std::string, graphics::ShaderProgram> shaderMap;
            public:
                Graphics(GLFWwindow* window);
        };
    }
#endif