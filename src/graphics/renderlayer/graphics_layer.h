#include <queue>
#include "render_layer.h"
#include "graphics/renderers/renderer.h"
#ifndef GAME_LAYER_H
#define GAME_LAYER_H
namespace graphics {
    class GraphicsRenderLayer : public RenderLayer {
    private:
        bool active = true;
        ShaderProgram* program;
        std::vector<RenderLayer*> renderLayers;
        //std::queue<int> bufferGets;
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

        std::optional<RenderLayer*> getRenderLayer (const std::string& layer);

        void addRenderLayer (RenderLayer* layer);
    };
}
#endif //GAME_LAYER_H
