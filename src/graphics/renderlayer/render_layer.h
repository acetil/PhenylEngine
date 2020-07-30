#include <string>

#include "graphics/graphics_new_include.h"
#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H
namespace graphics {
    class RenderLayer {
    public:
        virtual std::string getName () = 0;

        virtual int getPriority () = 0;

        virtual bool isActive () = 0;

        virtual BufferInfo getBufferInfo () = 0;

        virtual void addBuffer (BufferNew buf) = 0;

        virtual void gatherData () = 0;

        virtual void preRender (Renderer* renderer) = 0;

        virtual int getUniformId (std::string uniformName) = 0;

        virtual void applyUniform (int uniformId, void* data) = 0;

        virtual void applyCamera (CameraNew camera) = 0;

        virtual void render (Renderer* renderer, FrameBuffer* frameBuf) = 0;
    };
}
#endif //RENDER_LAYER_H
