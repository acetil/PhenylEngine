#include "graphics/textures/texture_atlas.h"
#include "graphics/renderlayer/render_layer.h"
#include "component/component.h"
#ifndef ENTITY_LAYER_H
#define ENTITY_LAYER_H
namespace graphics {
    class EntityRenderLayer : public RenderLayer {
    private:
        bool active = true;
        //Buffer buffer;
        //bool needsBuffer = true;
        component::EntityComponentManager::SharedPtr componentManager;
        ShaderProgram* shaderProgram;
        Buffer buffers[5];
        //int numBuffers = 0;
        GraphicsBufferIds buffIds;
        int numTriangles = 0;
    public:
        EntityRenderLayer (Renderer* renderer, component::EntityComponentManager::SharedPtr componentManager);

        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;
    };
}
#endif //ENTITY_LAYER_H
