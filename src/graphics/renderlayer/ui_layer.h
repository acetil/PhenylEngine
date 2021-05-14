#include "render_layer.h"

#ifndef UI_LAYER_H
#define UI_LAYER_H
namespace graphics {
#ifndef FONT_H
    class Font;
#endif
    class UIRenderLayer : public RenderLayer {
    private:
        GraphicsBufferIds textIds;
        Buffer textBuffer[2];
        GraphicsTexture fontTexture;
        ShaderProgram* textProgram;
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

        void bufferStr(Font& font, const std::string& text, int size, int x, int y);
    };
}
#endif
