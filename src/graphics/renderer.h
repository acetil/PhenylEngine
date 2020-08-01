#include <vector>
#include "buffer.h"
#include "shaders/shaders.h"
#ifndef RENDERER_H
#define RENDERER_H
namespace graphics {
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
        virtual double getCurrentTime () = 0;

        virtual bool shouldClose () = 0;

        virtual void pollEvents () = 0;

        virtual void clearWindow () = 0;

        virtual FrameBuffer* getWindowBuffer () = 0;
        virtual std::optional<ShaderProgram*> getProgram (std::string program) = 0;
        virtual GraphicsBufferIds getBufferIds (int requestedBufs, int bufferSize) = 0;
        virtual void bufferData (GraphicsBufferIds& ids, Buffer* buffers) = 0; // TODO: make more safe

        virtual void render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) = 0; // TODO: put rendering through frame buffer?

        virtual void finishRender () = 0;
    };
}
#endif //RENDERER_H
