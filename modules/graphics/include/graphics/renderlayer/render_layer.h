#pragma once

#include <string>

#include "graphics/renderers/renderer.h"
#include "graphics/camera.h"

namespace phenyl::graphics {
    class RenderLayer {
    // TODO: insert initial setup function
    public:
        virtual std::string getName () = 0;

        [[maybe_unused]] virtual int getPriority () = 0;

        virtual bool isActive () = 0;

        virtual void gatherData () = 0;

        virtual void preRender (Renderer* renderer) = 0;

        virtual int getUniformId (std::string uniformName) = 0;

        [[maybe_unused]] virtual void applyUniform (int uniformId, void* data) = 0;

        virtual void applyCamera (Camera camera) = 0;

        virtual void render (Renderer* renderer, FrameBuffer* frameBuf) = 0;

        virtual ~RenderLayer() = default;
    };
}
