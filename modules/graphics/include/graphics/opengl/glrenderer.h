#pragma once

#include "graphics/graphics_new_include.h"

#include "graphics/graphics_headers.h"

#include <unordered_map>
#include <memory>

#include "graphics/renderers/renderer.h"


namespace graphics {
    class GLFrameBuffer : public FrameBuffer {
    public:
        void bind () override {
            // TODO: do something
        }
    };

    class GLFWKeyInput;
    class GLFWMouseInput;
    class GLFWInput;

    class GLWindowCallbackCtx;

    class GLRenderer : public Renderer {
    private:
        GLFWwindow* window;
        std::shared_ptr<GLFrameBuffer> windowBuf;
        std::unordered_map<std::string, ShaderProgram*> shaderPrograms;
        util::Map<std::string, ShaderProgramNew> shaderProgramsNew;

        std::unique_ptr<WindowCallbackContext> callbackCtx;

        std::shared_ptr<GLFWInput> keyInput;
        std::shared_ptr<GLFWInput> mouseInput;

        std::unique_ptr<GLWindowCallbackCtx> windowCallbackCtx;

        glm::vec2 screenSize;

    protected:
        std::shared_ptr<RendererBufferHandle> makeBufferHandle() override;

    public:
        explicit GLRenderer (GLFWwindow* window);

        ~GLRenderer() override;

        double getCurrentTime () override;

        bool shouldClose () override;

        void pollEvents () override;

        void clearWindow () override;

        FrameBuffer* getWindowBuffer () override;

       //std::optional<ShaderProgram*> getProgram (std::string program) override;

        util::Optional<ShaderProgramNew> getProgramNew (const std::string& program) override;

        void finishRender () override;

        void addShader(const std::string &shaderName, const ShaderProgramBuilder& shaderBuilder) override;

        void addShader (const std::string& name, ShaderProgram* program);

        GLFWwindow* getWindow () {
            return window; // TODO: remove
        }
        void setupErrorHandling ();

        GraphicsTexture loadTexture (int width, int height, unsigned char* data) override;
        GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) override;

        void bindTexture (unsigned int textureId) override;

        void setupWindowCallbacks (std::unique_ptr<WindowCallbackContext> ctx) override;
        void setupCallbacks(const std::shared_ptr<event::EventBus> &eventBus) override;

        glm::vec2 getScreenSize() override;
        void setScreenSize (glm::vec2 screenSize);

        void invalidateWindowCallbacks () override;

        PipelineStage buildPipelineStage(const PipelineStageBuilder &stageBuilder) override;

        std::shared_ptr<common::InputSource> getMouseInput() override;

        std::vector<std::shared_ptr<common::InputSource>> getInputSources () override;

        void onKeyChange (int key, int scancode, int action, int mods);
        void onMouseButtonChange (int button, int action, int mods);
    };
}
