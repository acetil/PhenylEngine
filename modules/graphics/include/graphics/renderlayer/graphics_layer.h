#pragma once

#include <queue>
#include <memory>

#include "graphics/font/glyph_atlas.h"
#include "graphics/font/font.h"
#include "graphics/font/font_manager.h"
#include "graphics/renderlayer/render_layer.h"
#include "graphics/renderers/renderer.h"

namespace graphics {
    class GraphicsRenderLayer : public RenderLayer {
    private:
        bool active = true;
        //Shader program;
        std::vector<std::shared_ptr<RenderLayer>> renderLayers;
    public:
        explicit GraphicsRenderLayer(Renderer* renderer);
        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;

        std::optional<std::shared_ptr<RenderLayer>> getRenderLayer (const std::string& layer);

        void addRenderLayer (std::shared_ptr<RenderLayer> layer);
    };
}
