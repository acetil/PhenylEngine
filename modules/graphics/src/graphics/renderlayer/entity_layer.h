#pragma once

#include "graphics/renderlayer/render_layer.h"
#include "component/component.h"
#include "graphics/pipeline/pipeline.h"
#include "graphics/textures/sprite_atlas.h"

namespace phenyl::graphics {
    class EntityPipeline;
    class EntityRenderLayer : public RenderLayer {
    private:
        bool active = true;
        component::EntityComponentManager* componentManager;

        std::unique_ptr<EntityPipeline> entityPipeline;
        SpriteAtlas atlas;
    public:
        EntityRenderLayer (Renderer* renderer, component::EntityComponentManager* componentManager);
        ~EntityRenderLayer() override;

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
