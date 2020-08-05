#include <string>

#include "graphics/graphics_new_include.h"
#include "graphics/renderer.h"
#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H
namespace graphics {
    class RenderLayer {
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
    };
}
#endif //RENDER_LAYER_H
