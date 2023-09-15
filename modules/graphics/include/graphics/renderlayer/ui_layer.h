#pragma once

#include "graphics/renderlayer/render_layer.h"
#include "graphics/maths_headers.h"
#include "graphics/pipeline/pipeline.h"
#include "graphics/ui/ui_rect.h"

namespace graphics {
//#ifndef FONT_H
    class Font;
//#endif
    class RenderedText;

    class UIPipeline : public Pipeline<> {
    public:
        virtual void bufferText (const RenderedText& text) = 0;
        virtual void bufferRect (const UIRect& rect) = 0;
        virtual void setScreenSize (glm::vec2 screenSize) = 0;
    };

    class UIRenderLayer : public RenderLayer {
    private:
        //GraphicsTexture fontTexture;
        ShaderProgramNew textProgram;
        std::unique_ptr<UIPipeline> pipeline;
    public:
        explicit UIRenderLayer(GraphicsTexture _fontTexture, Renderer* renderer);

        std::string getName () override;

        [[maybe_unused]] int getPriority () override;

        bool isActive () override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        [[maybe_unused]] void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;

        void bufferStr(Font& font, const std::string& text, int size, int x, int y, glm::vec3 colour);

        void bufferText (const RenderedText& text);

        void bufferRect (const UIRect& rect);

        void setScreenSize (glm::vec2 screenSize);
    };
}
