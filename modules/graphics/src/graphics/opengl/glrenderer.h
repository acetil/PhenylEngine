#pragma once

#include <unordered_map>
#include <memory>

#include "graphics/renderers/renderer.h"
#include "graphics/shaders/shaders.h"
#include "graphics/graphics_headers.h"

#include "glshader.h"
#include "graphics/glfw/glfw_viewport.h"


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
        std::unique_ptr<GLFWViewport> viewport;
        std::shared_ptr<GLFrameBuffer> windowBuf;
        util::Map<std::string, Shader> shaderProgramsNew;

        GLShaderManager shaderManager;

        glm::vec2 screenSize;
        glm::vec2 mousePos;

        common::Asset<Shader> boxShader;
        common::Asset<Shader> debugShader;
        common::Asset<Shader> spriteShader;
        common::Asset<Shader> textShader;
        common::Asset<Shader> particleShader;
    protected:
        std::unique_ptr<IBuffer> makeRendererBuffer (std::size_t startCapacity) override;

    public:
        explicit GLRenderer (std::unique_ptr<GLFWViewport> viewport);

        static std::unique_ptr<GLRenderer> Make (const GraphicsProperties& properties);

        ~GLRenderer() override;

        std::string_view getName() const noexcept override;

        double getCurrentTime () override;

        //bool shouldClose () override;

        //void pollEvents () override;

        void clearWindow () override;

        FrameBuffer* getWindowBuffer () override;

       //std::optional<ShaderProgram*> getProgram (std::string program) override;

        //util::Optional<Shader> getProgramNew (const std::string& program) override;

        void finishRender () override;

        //void addShader(const std::string &shaderName, const ShaderBuilder& shaderBuilder) override;

        void setupErrorHandling ();

        GraphicsTexture loadTexture (int width, int height, unsigned char* data) override;
        GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) override;

        void bindTexture (unsigned int textureId) override;
        void reloadTexture(unsigned int textureId, int width, int height, unsigned char *data) override;

        void destroyTexture(unsigned int textureId) override;

        //[[nodiscard]] glm::vec2 getScreenSize() const override;
        //[[nodiscard]] glm::vec2 getMousePos() const override;
        void setScreenSize (glm::vec2 screenSize);


        PipelineStage buildPipelineStage (PipelineStageBuilder& builder) override;
        void loadDefaultShaders() override;

        //std::vector<std::shared_ptr<common::InputSource>> getInputSources () override;
        //std::vector<std::shared_ptr<common::ProxySource>> getProxySources() override;

        Viewport& getViewport() override;
        const Viewport& getViewport() const override;
    };
}
