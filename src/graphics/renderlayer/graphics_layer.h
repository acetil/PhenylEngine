#include <queue>
#include "graphics/font/glyph_atlas.h"
#include "graphics/font/font.h"
#include "graphics/font/font_manager.h"
#include "render_layer.h"
#include "graphics/renderers/renderer.h"
#ifndef GAME_LAYER_H
#define GAME_LAYER_H
namespace graphics {
    class GraphicsRenderLayer : public RenderLayer {
    private:
        bool active = true;
        ShaderProgram* program;
        ShaderProgram* textProgram;
        std::vector<RenderLayer*> renderLayers;
        GraphicsBufferIds ids;
        Buffer buffers[2];
        GraphicsTexture tex;
        FontManager manager;
        Font font;
        float scale = 0.1f;
        //std::queue<int> bufferGets;
    public:
        explicit GraphicsRenderLayer(Renderer* renderer, FontManager& manager);
        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;

        std::optional<RenderLayer*> getRenderLayer (const std::string& layer);

        void addRenderLayer (RenderLayer* layer);
        void addTex (GraphicsTexture _tex) {
            tex = _tex; // TODO: remove
        };
    };
}
#endif //GAME_LAYER_H
