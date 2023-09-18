#pragma once

#include "graphics/renderlayer/render_layer.h"

namespace graphics {
    class DebugPipeline;
    class DebugLayer : public RenderLayer {
    private:
        bool active = true;
        std::unique_ptr<DebugPipeline> pipeline;
        glm::vec2 screenSize;
    public:
        explicit DebugLayer (Renderer* renderer);

        std::string getName() override;
        int getPriority() override;
        bool isActive() override;
        void gatherData() override;
        void preRender(graphics::Renderer *renderer) override;
        int getUniformId(std::string uniformName) override;
        void applyUniform(int uniformId, void *data) override;
        void applyCamera(graphics::Camera camera) override;
        void render(graphics::Renderer *renderer, graphics::FrameBuffer *frameBuf) override;
    };

    std::shared_ptr<DebugLayer> makeDebugLayer (Renderer* renderer);
}