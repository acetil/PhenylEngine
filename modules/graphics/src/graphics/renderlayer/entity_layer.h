#pragma once

#include "graphics/textures/texture_atlas.h"
#include "graphics/renderlayer/render_layer.h"
#include "component/component.h"
#include "component/component_new.h"
#include "graphics/pipeline/pipeline.h"

namespace graphics {
    class EntityPipeline;
    class EntityRenderLayer : public RenderLayer {
    private:
        bool active = true;
        component::EntityComponentManager::SharedPtr componentManager;
        ShaderProgramNew shaderProgram;

        std::unique_ptr<EntityPipeline> entityPipeline;
    public:
        EntityRenderLayer (Renderer* renderer, component::EntityComponentManager::SharedPtr componentManager);
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
