#pragma once

#include "graphics/renderlayer/render_layer.h"

namespace phenyl::graphics {
    class ParticlePipeline;
    class ParticleManager2D;

    class ParticleRenderLayer : public RenderLayer {
    private:
        std::unique_ptr<ParticlePipeline> pipeline;
        ParticleManager2D* manager;
    public:
        explicit ParticleRenderLayer (phenyl::graphics::Renderer* renderer, ParticleManager2D* manager);
        ~ParticleRenderLayer() override;

        std::string getName () override;
        int getPriority () override;
        bool isActive () override;
        void gatherData () override;
        void preRender (phenyl::graphics::Renderer* renderer) override;
        int getUniformId (std::string uniformName) override;
        void applyUniform (int uniformId, void* data) override;
        void applyCamera (phenyl::graphics::Camera camera) override;
        void render (phenyl::graphics::Renderer* renderer, phenyl::graphics::FrameBuffer* frameBuf) override;
    };
}