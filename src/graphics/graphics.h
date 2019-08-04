#include "graphics_headers.h"
#include "shaders/shaders.h"
#include "camera.h"
#include "texture.h"
#include <vector>
#include <unordered_map>
#ifndef GRAPHICS_H
    #define GRAPHICS_H
    namespace graphics {
        class Buffer {
            private:
                float* vertexPosData;
                float* vertexUvData;
                float* vertexPosCurrent;
                float* vertexUvCurrent;
                GLuint posBufferId;
                GLuint uvBufferId;
                size_t posBufferSize;
                size_t uvBufferSize;
                unsigned int numSprites;
                unsigned int maxNumSprites;
            public:
                void initBuffer (unsigned int numSprites);
                void reinitBuffer (unsigned int numSprites);
                void pushBuffer (float* posData, float* uvData);
                void flushBuffer (ShaderProgram shader, Camera camera);
        };
        class Graphics {
            private:
                GLFWwindow* window;
                std::unordered_map<std::string, graphics::ShaderProgram> shaderMap;
                int numImages;
                Buffer spriteBuffer;
            public:
                Graphics(GLFWwindow* window);
                bool shouldClose ();
                void pollEvents ();
                void render ();
                void drawTexSquare (float x, float y, Texture tex);
        };
    }
#endif