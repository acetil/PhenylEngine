#include "graphics/graphics_new_include.h"

#include "graphics/graphics_headers.h"

#include <unordered_map>
#include <memory>

#include "renderer.h"
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
        std::shared_ptr<GLFrameBuffer> windowBuf;
        std::unordered_map<std::string, ShaderProgram*> shaderPrograms;

        std::unique_ptr<WindowCallbackContext> callbackCtx;

    public:
        explicit GLRenderer (GLFWwindow* window);

        ~GLRenderer() override;

        double getCurrentTime () override;

        bool shouldClose () override;

        void pollEvents () override;

        void clearWindow () override;

        FrameBuffer* getWindowBuffer () override;

        std::optional<ShaderProgram*> getProgram (std::string program) override;

        GraphicsBufferIds getBufferIds (int requestedBufs, int bufferSize, std::vector<int> attribSizes) override;

        void bufferData (GraphicsBufferIds& ids, Buffer* buffers) override; // TODO: make more safe

        void render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) override; // TODO: put rendering through frame buffer?

        void finishRender () override;

        void addShader (const std::string& name, ShaderProgram* program);

        GLFWwindow* getWindow () {
            return window; // TODO: remove
        }
        void setupErrorHandling ();

        GraphicsTexture loadTexture (int width, int height, unsigned char* data) override;
        GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) override;

        void bindTexture (unsigned int textureId) override;

        void setupWindowCallbacks (std::unique_ptr<WindowCallbackContext> ctx) override;

        void invalidateWindowCallbacks () override;
    };
}
#endif //GLRENDERER_H
