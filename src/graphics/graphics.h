#include "graphics_headers.h"
#include "shaders/shaders.h"
#include "camera.h"
#include "textures/texture.h"
#include "textures/texture_atlas.h"
#include "component/main_component.h"
#include "event/events/entity_creation.h"
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#ifndef GRAPHICS_H
    #define GRAPHICS_H
    #ifndef KEY_INPUT_H
    namespace game {
        class KeyboardInput;
    }
    #endif
    namespace graphics {
        // TODO: expose only the interface in this header
        // TODO: refactor into multiple files
        class Graphics;
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
                unsigned int numSpritesVertex;
                unsigned int numSpritesUv;
                unsigned int maxNumSprites;
            public:
                void initBuffer (unsigned int numSprites);
                void reinitBuffer (unsigned int numSprites);
                float* getVertexBufferPos ();
                float* getUvBufferPos();
                void incrementVertexPtr (int amount);
                void incrementUvPtr (int amount); 
                void pushBuffer (float* posData, float* uvData);
                void flushBuffer (ShaderProgram* shader, Camera* camera, TextureAtlas* textureAtlas);
                ~Buffer();
        };
        class StaticData {
            private:
            GLuint vertexVBO;
            GLuint uvVBO;
            int numVertices;
            int vertexComp;
            int uvComp;
            ShaderProgram* shaderProgram;
            friend class Graphics;
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
                int fps;
                double lastTime;
                std::vector<StaticData*> staticData;
            public:
                Graphics(GLFWwindow* window);
                ~Graphics();
                bool shouldClose ();
                void pollEvents ();
                void render ();
                void renderStaticData ();
                void drawTexSquare (float x, float y, float scale, int textureId);
                Camera* getCamera ();
                void addShader (std::string name, ShaderProgram* shader);
                void setCurrentSpriteShader (std::string name);
                void initSpriteAtlas (std::vector<Image*> images);
                int getSpriteTextureId (std::string name);
                void initBuffer (unsigned int numSprites);
                void startTimer (int fps);
                float getDeltaTime ();
                Buffer* getSpriteBuffer ();
                StaticData* loadStaticData (float* vertexArray, float* uvArray, int numVertices, int sizeVertex, int sizeUv, 
                    std::string shader);
                void unloadStaticData (StaticData* data);
                void setupErrorHandling ();
                void bufferEntityPositions (component::EntityMainComponent* comp, int numEntities, int direction, Buffer* buffer);
                TextureAtlas* getTextureAtlas ();
                void onEntityCreation (event::EntityCreationEvent* event);
                friend class game::KeyboardInput;
        };
    }
#endif