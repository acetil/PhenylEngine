#include "render_layer.h"
#include "component/component.h"
#ifndef ENTITY_LAYER_H
#define ENTITY_LAYER_H
namespace graphics {
    class EntityRenderLayer : public RenderLayer {
    private:
        bool active = true;
        BufferNew buffer;
        bool needsBuffer = true;
        component::ComponentManager<game::AbstractEntity*>* componentManager;
        ShaderProgram* shaderProgram;
        BufferNew buffers[2];
        int numBuffers = 0;
        GraphicsBufferIds buffIds;
    public:
        EntityRenderLayer (Renderer* renderer, component::ComponentManager<game::AbstractEntity*>* componentManager);

        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        BufferInfo getBufferInfo () override;

        void addBuffer (BufferNew buf) override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (CameraNew camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;
    };
}
#endif //ENTITY_LAYER_H
