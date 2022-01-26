#pragma once

#include "engine/map/map.h"
#include "render_layer.h"
#include "event/events/map_load.h"

namespace graphics {
    class MapRenderLayer : public RenderLayer {
    private:
        bool active = false;
        Buffer buffers[5];
        //bool requiresBuffer = true;
        game::Map::SharedPtr map;
        GraphicsBufferIds bufferIds;
        bool needDataBuffer = true;
        int numTriangles;
        ShaderProgram* program;
        TextureAtlas& atlas;
    public:
        explicit MapRenderLayer(Renderer* renderer, TextureAtlas& atlas);
        std::string getName () override;

        int getPriority () override;

        bool isActive () override;

        void gatherData () override;

        void preRender (Renderer* renderer) override;

        int getUniformId (std::string uniformName) override;

        void applyUniform (int uniformId, void* data) override;

        void applyCamera (Camera camera) override;

        void render (Renderer* renderer, FrameBuffer* frameBuf) override;

        void attachMap (game::Map::SharedPtr map);

        void onMapLoad (event::MapLoadEvent& event);
    };

};
