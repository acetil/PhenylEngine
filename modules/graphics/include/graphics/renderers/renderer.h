#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "graphics/buffer.h"
#include "graphics/shaders/shaders.h"

namespace graphics {
//#ifndef WINDOW_CALLBACKS_H
    class WindowCallbackContext;
//#endif
    class GraphicsTexture;
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };
    // TODO: roll into Buffer
    struct GraphicsBufferIds {
        unsigned int vaoId;
        std::vector<unsigned int> vboIds;
        GraphicsBufferIds (): vaoId(0), vboIds({}) {};
        GraphicsBufferIds (unsigned int _vaoId, std::vector<unsigned int> _vboIds): vaoId(_vaoId), vboIds(std::move(_vboIds)) {};
    };

    class Renderer {
    public:

        virtual ~Renderer() = default;

        virtual double getCurrentTime () = 0;

        virtual bool shouldClose () = 0;

        virtual void pollEvents () = 0;

        virtual void clearWindow () = 0;

        virtual FrameBuffer* getWindowBuffer () = 0;
        virtual std::optional<ShaderProgram*> getProgram (std::string program) = 0;
        virtual GraphicsBufferIds getBufferIds (int requestedBufs, int bufferSize, std::vector<int> attribSizes) = 0;
        virtual void bufferData (GraphicsBufferIds& ids, Buffer* buffers) = 0; // TODO: make more safe

        virtual void render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) = 0; // TODO: put rendering through frame buffer?

        virtual void finishRender () = 0;

        virtual GraphicsTexture loadTexture (int width, int height, unsigned char* data) = 0;
        virtual GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) = 0;

        virtual void bindTexture (unsigned int textureId) = 0;

        virtual void setupWindowCallbacks (std::unique_ptr<WindowCallbackContext> ctx) = 0;

        virtual void invalidateWindowCallbacks () = 0;
    };
    class GraphicsTexture {
        Renderer* renderer;
        unsigned int textureId;
    public:
        GraphicsTexture () : renderer(nullptr), textureId (-1) {};
        GraphicsTexture (Renderer* render, unsigned int id) : renderer(render), textureId(id) {};
        void bindTexture () {
            renderer->bindTexture(textureId);
        }
    };

}
