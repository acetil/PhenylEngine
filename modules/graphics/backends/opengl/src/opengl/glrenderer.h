#pragma once

#include "glframebuffer.h"
#include "glfw/glfw_viewport.h"
#include "glshader.h"
#include "graphics/backend/renderer.h"
#include "opengl_headers.h"

#include <memory>
#include <unordered_map>

namespace phenyl::opengl {
class GLNoopCommandList : public graphics::ICommandList {};

class GLRenderer : public graphics::Renderer, public graphics::IViewportUpdateHandler {
public:
    static std::unique_ptr<GLRenderer> Make (const graphics::GraphicsProperties& properties);
    explicit GLRenderer (std::unique_ptr<glfw::GLFWViewport> viewport);

    void setupErrorHandling ();

    std::string_view getName () const noexcept override;

    double getCurrentTime () override;

    void clearWindow () override;
    void render () override;
    void finishRender () override;

    graphics::PipelineBuilder buildPipeline () override;
    void loadDefaultShaders () override;

    graphics::Viewport& getViewport () override;
    const graphics::Viewport& getViewport () const override;

    void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;

protected:
    std::unique_ptr<graphics::IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize,
        graphics::BufferStorageHint storageHint, bool isIndex) override;
    std::unique_ptr<graphics::IUniformBuffer> makeRendererUniformBuffer (bool readable) override;
    std::unique_ptr<graphics::IImageTexture> makeRendererImageTexture (
        const graphics::TextureProperties& properties) override;
    std::unique_ptr<graphics::IImageArrayTexture> makeRendererArrayTexture (
        const graphics::TextureProperties& properties, std::uint32_t width, std::uint32_t height) override;
    std::unique_ptr<graphics::IFrameBuffer> makeRendererFrameBuffer (const graphics::FrameBufferProperties& properties,
        std::uint32_t width, std::uint32_t height) override;
    graphics::ICommandList* makeCommandList () override;

private:
    std::unique_ptr<glfw::GLFWViewport> m_viewport;
    GlWindowFrameBuffer m_windowFrameBuffer;
    glm::vec4 m_clearColor;

    GlShaderManager m_shaderManager;
    GLNoopCommandList m_noopCommandList{};

    std::shared_ptr<graphics::Shader> m_boxShader;
    std::shared_ptr<graphics::Shader> m_debugShader;
    std::shared_ptr<graphics::Shader> m_spriteShader;
    std::shared_ptr<graphics::Shader> m_textShader;
    std::shared_ptr<graphics::Shader> m_particleShader;
    std::shared_ptr<graphics::Shader> m_meshShader;
    std::shared_ptr<graphics::Shader> m_shadowMapShader;
    std::shared_ptr<graphics::Shader> m_prepassShader;

    std::shared_ptr<graphics::Shader> m_noopPostShader;
};
} // namespace phenyl::opengl
