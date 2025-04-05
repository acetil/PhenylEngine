#pragma once

#include <memory>
#include <unordered_map>

#include "glframebuffer.h"
#include "opengl_headers.h"
#include "graphics/backend/renderer.h"

#include "glshader.h"
#include "glfw/glfw_viewport.h"

namespace phenyl::opengl {
    class GLRenderer : public graphics::Renderer, public graphics::IViewportUpdateHandler {
    private:
        std::unique_ptr<glfw::GLFWViewport> viewport;
        GlWindowFrameBuffer windowFrameBuffer;
        glm::vec4 clearColor;

        GlShaderManager shaderManager;

        core::Asset<graphics::Shader> boxShader;
        core::Asset<graphics::Shader> debugShader;
        core::Asset<graphics::Shader> spriteShader;
        core::Asset<graphics::Shader> textShader;
        core::Asset<graphics::Shader> particleShader;
        core::Asset<graphics::Shader> meshShader;
        core::Asset<graphics::Shader> shadowMapShader;
        core::Asset<graphics::Shader> prepassShader;

        core::Asset<graphics::Shader> noopPostShader;
    protected:
        std::unique_ptr<graphics::IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize, bool isIndex) override;
        std::unique_ptr<graphics::IUniformBuffer> makeRendererUniformBuffer (bool readable) override;
        std::unique_ptr<graphics::IImageTexture> makeRendererImageTexture (const graphics::TextureProperties& properties) override;
        std::unique_ptr<graphics::IImageArrayTexture> makeRendererArrayTexture(const graphics::TextureProperties &properties, std::uint32_t width, std::uint32_t height) override;
        std::unique_ptr<graphics::IFrameBuffer> makeRendererFrameBuffer (const graphics::FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) override;

    public:
        static std::unique_ptr<GLRenderer> Make (const graphics::GraphicsProperties& properties);
        explicit GLRenderer (std::unique_ptr<glfw::GLFWViewport> viewport);

        void setupErrorHandling ();

        std::string_view getName() const noexcept override;

        double getCurrentTime () override;

        void clearWindow () override;
        void render () override;
        void finishRender () override;

        graphics::PipelineBuilder buildPipeline () override;
        void loadDefaultShaders () override;

        graphics::Viewport& getViewport () override;
        const graphics::Viewport& getViewport () const override;

        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    };
}
