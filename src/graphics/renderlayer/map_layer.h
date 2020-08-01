#include "game/map/map.h"
#include "render_layer.h"
#ifndef MAP_LAYER_H
#define MAP_LAYER_H
namespace graphics {
    class MapRenderLayer : public RenderLayer {
    private:
        bool active = false;
        BufferNew buffers[2];
        bool requiresBuffer = true;
        game::Map* map;
        GraphicsBufferIds bufferIds;
        bool needDataBuffer = true;
        int numTriangles;
        ShaderProgram* program;
    public:
        explicit MapRenderLayer(Renderer* renderer);
        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        BufferInfo getBufferInfo () override;

        void addBuffer (BufferNew buf) override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;

        void attachMap (game::Map* map);
    };

};
#endif //MAP_LAYER_H
