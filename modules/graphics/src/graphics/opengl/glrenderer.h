#pragma once

#include <memory>
#include <unordered_map>

#include "graphics/graphics_headers.h"
#include "graphics/renderer.h"

#include "glshader.h"
#include "graphics/glfw/glfw_viewport.h"


namespace phenyl::graphics {
    class GLRenderer : public Renderer, public IViewportUpdateHandler {
    private:
        std::unique_ptr<GLFWViewport> viewport;

        GlShaderManager shaderManager;

        core::Asset<Shader> boxShader;
        core::Asset<Shader> debugShader;
        core::Asset<Shader> spriteShader;
        core::Asset<Shader> textShader;
        core::Asset<Shader> particleShader;
        core::Asset<Shader> meshShader;
        core::Asset<Shader> prepassShader;
    protected:
        std::unique_ptr<IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) override;
        std::unique_ptr<IUniformBuffer> makeRendererUniformBuffer (bool readable) override;
        std::unique_ptr<IImageTexture> makeRendererImageTexture (const TextureProperties& properties) override;
        std::unique_ptr<IImageArrayTexture> makeRendererArrayTexture(const TextureProperties &properties, std::uint32_t width, std::uint32_t height) override;

    public:
        static std::unique_ptr<GLRenderer> Make (const GraphicsProperties& properties);
        explicit GLRenderer (std::unique_ptr<GLFWViewport> viewport);

        void setupErrorHandling ();

        std::string_view getName() const noexcept override;

        double getCurrentTime () override;

        void clearWindow () override;
        void render () override;
        void finishRender () override;

        PipelineBuilder buildPipeline () override;
        void loadDefaultShaders () override;

        Viewport& getViewport () override;
        const Viewport& getViewport () const override;

        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    };
}
