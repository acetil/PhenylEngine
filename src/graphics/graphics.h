#include "graphics_headers.h"
#include "shaders/shaders.h"
#include "camera.h"
#include "textures/texture.h"
#include "textures/texture_atlas.h"
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#ifndef GRAPHICS_H
    #define GRAPHICS_H
    namespace graphics {
        class Buffer {
            private:
                float* vertexPosData = NULL;
                float* vertexUvData = NULL;
                float* vertexPosCurrent = NULL;
                float* vertexUvCurrent = NULL;
                GLuint posBufferId;
                GLuint uvBufferId;
                GLuint vaoId;
                size_t posBufferSize;
                size_t uvBufferSize;
                unsigned int numSprites;
                unsigned int maxNumSprites;
            public:
                void initBuffer (unsigned int numSprites);
                void reinitBuffer (unsigned int numSprites);
                void pushBuffer (float* posData, float* uvData);
                void flushBuffer (ShaderProgram* shader, Camera* camera, TextureAtlas* textureAtlas);
                ~Buffer();
        };
        class Graphics {
            private:
                GLFWwindow* window;
                std::unordered_map<std::string, graphics::ShaderProgram*> shaderMap;
                int numImages;
                Buffer* spriteBuffer;
                TextureAtlas* spriteAtlas; 
                Camera* camera;
                ShaderProgram* currentSpriteShader; // TODO: remove
            public:
                Graphics(GLFWwindow* window);
                ~Graphics();
                bool shouldClose ();
                void pollEvents ();
                void render ();
                void drawTexSquare (float x, float y, float scale, int textureId);
                Camera* getCamera ();
                void addShader (std::string name, ShaderProgram* shader);
                void setCurrentSpriteShader (std::string name);
                void initSpriteAtlas (std::vector<Image*> images);
                int getSpriteTextureId (std::string name);
                void initBuffer (unsigned int numSprites);
        };
    }
#endif