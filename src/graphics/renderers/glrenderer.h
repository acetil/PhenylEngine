#include "graphics/graphics_new_include.h"

#include "graphics/graphics_headers.h"

#include <unordered_map>

#ifndef GLRENDERER_H
#define GLRENDERER_H
namespace graphics {
    class GLFrameBuffer : public FrameBuffer {
    public:
        void bind () override {
            // TODO: do something
        }
    };

    class GLRenderer : public Renderer {
    private:
        GLFWwindow* window;
        GLFrameBuffer* windowBuf;
        std::unordered_map<std::string, ShaderProgram*> shaderPrograms;

    public:
        explicit GLRenderer (GLFWwindow* window);

        double getCurrentTime () override;

        bool shouldClose () override;

        void pollEvents () override;

        void clearWindow () override;

        FrameBuffer* getWindowBuffer () override;

        std::optional<ShaderProgram*> getProgram (std::string program) override;

        GraphicsBufferIds getBufferIds (int requestedBufs, int bufferSize) override;

        void bufferData (GraphicsBufferIds& ids, BufferNew* buffers) override; // TODO: make more safe

        void render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) override; // TODO: put rendering through frame buffer?

        void finishRender () override;

        void addShader (const std::string& name, ShaderProgram* program);

        GLFWwindow* getWindow () {
            return window; // TODO: remove
        }
        void setupErrorHandling ();
    };
}
#endif //GLRENDERER_H
