#pragma once

#include <unordered_map>
#include <memory>

#include "graphics/renderers/renderer.h"
#include "graphics/shaders/shaders.h"
#include "graphics/graphics_headers.h"

#include "glshader.h"


namespace phenyl::graphics {
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
        util::Map<std::string, Shader> shaderProgramsNew;

        std::shared_ptr<GLFWInput> keyInput;
        std::shared_ptr<GLFWInput> mouseInput;

        std::unique_ptr<GLWindowCallbackCtx> windowCallbackCtx;
        GLShaderManager shaderManager;

        glm::vec2 screenSize;
        glm::vec2 mousePos;

        common::Asset<Shader> boxShader;
        common::Asset<Shader> debugShader;
        common::Asset<Shader> spriteShader;
        common::Asset<Shader> textShader;
        common::Asset<Shader> particleShader;
    protected:
        std::shared_ptr<RendererBufferHandle> makeBufferHandle() override;

    public:
        explicit GLRenderer (GLFWwindow* window);

        ~GLRenderer() override;

        std::string_view getName() const noexcept override;

        double getCurrentTime () override;

        bool shouldClose () override;

        void pollEvents () override;

        void clearWindow () override;

        FrameBuffer* getWindowBuffer () override;

       //std::optional<ShaderProgram*> getProgram (std::string program) override;

        //util::Optional<Shader> getProgramNew (const std::string& program) override;

        void finishRender () override;

        //void addShader(const std::string &shaderName, const ShaderBuilder& shaderBuilder) override;

        GLFWwindow* getWindow () {
            return window; // TODO: remove
        }
        void setupErrorHandling ();

        GraphicsTexture loadTexture (int width, int height, unsigned char* data) override;
        GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) override;

        void bindTexture (unsigned int textureId) override;
        void reloadTexture(unsigned int textureId, int width, int height, unsigned char *data) override;

        void destroyTexture(unsigned int textureId) override;

        void setupCallbacks () override;

        bool mouseDown() const override;
        [[nodiscard]] glm::vec2 getScreenSize() const override;
        [[nodiscard]] glm::vec2 getMousePos() const override;
        void setScreenSize (glm::vec2 screenSize);

        void invalidateWindowCallbacks () override;

        PipelineStage buildPipelineStage (PipelineStageBuilder& builder) override;
        void loadDefaultShaders() override;

        std::shared_ptr<common::InputSource> getMouseInput() override;

        std::vector<std::shared_ptr<common::InputSource>> getInputSources () override;

        void onKeyChange (int key, int scancode, int action, int mods);
        void onMouseButtonChange (int button, int action, int mods);
        void onMousePosChange (glm::vec2 newPos);
    };
}
