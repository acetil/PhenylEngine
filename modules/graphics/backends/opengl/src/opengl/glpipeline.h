#pragma once

#include "glframebuffer.h"
#include "util/map.h"

#include "graphics/backend/pipeline.h"
#include "opengl_headers.h"
#include "graphics/backend/shader.h"
#include "glshader.h"

namespace phenyl::opengl {
    class GlPipeline : public graphics::IPipeline {
    private:
        struct PipelineIndex {
            GLenum typeEnum;
            std::size_t typeSize;
        };

        GLuint vaoId;
        GLenum renderMode = GL_TRIANGLES;
        GlWindowFrameBuffer* windowFrameBuffer;
        core::Asset<graphics::Shader> shader;
        std::vector<std::size_t> bufferTypes;
        util::Map<graphics::UniformBinding, std::size_t> uniformTypes;
        std::optional<PipelineIndex> indexType = std::nullopt;

        bool doDepthTest = false;
        bool doDepthMask = false;
        graphics::BlendMode blendMode = graphics::BlendMode::ALPHA_BLEND;
        graphics::CullMode cullMode = graphics::CullMode::NONE;

        GlShader& getShader ();
        void updateDepthMask ();
        void setBlending (const AbstractGlFrameBuffer& fb);
        void setCulling ();
        void bindFrameBuffer (graphics::IFrameBuffer* frameBuffer);
    public:
        explicit GlPipeline (GlWindowFrameBuffer* fb);
        GlPipeline (const GlPipeline&) = delete;
        GlPipeline (GlPipeline&& other) noexcept;

        GlPipeline& operator= (const GlPipeline&) = delete;
        GlPipeline& operator= (GlPipeline&& other) noexcept;

        ~GlPipeline () override;

        void bindBuffer (std::size_t type, graphics::BufferBinding binding, const graphics::IBuffer& buffer, std::size_t offset) override;
        void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
        void bindUniform (std::size_t type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer, std::size_t offset, std::size_t size) override;
        void bindSampler (graphics::SamplerBinding binding, graphics::ISampler& sampler) override;
        void unbindIndexBuffer () override;

        void render (graphics::IFrameBuffer* frameBuffer, std::size_t vertices, std::size_t offset) override;
        void renderInstanced (graphics::IFrameBuffer* frameBuffer, std::size_t numInstances, std::size_t vertices, std::size_t offset) override;

        void setRenderMode (GLenum renderMode);
        void setShader (core::Asset<graphics::Shader> shader);

        graphics::BufferBinding addBuffer (std::size_t type, GLuint divisor);
        void addAttrib (GLenum type, GLint size, GLuint location, graphics::BufferBinding binding, std::size_t offset);

        graphics::UniformBinding addUniform (std::size_t type, unsigned int location);
        graphics::SamplerBinding addSampler (unsigned int location);

        void setDepthTest (bool doMask);
        void setBlendMode (graphics::BlendMode mode);
        void setCullMode (graphics::CullMode mode);
    };

    class GlPipelineBuilder : public graphics::IPipelineBuilder {
    private:
        std::unique_ptr<GlPipeline> pipeline;
    public:
        GlPipelineBuilder (GlWindowFrameBuffer* fb);

        void withGeometryType (graphics::GeometryType type) override;
        void withShader (core::Asset<graphics::Shader> shader) override;

        graphics::BufferBinding withBuffer(std::size_t type, std::size_t size, graphics::BufferInputRate inputRate) override;
        void withAttrib(graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding, std::size_t offset) override;

        graphics::UniformBinding withUniform (std::size_t type, unsigned int location) override;
        graphics::SamplerBinding withSampler (unsigned int location) override;

        void withCullMode(graphics::CullMode mode) override;
        void withBlendMode (graphics::BlendMode mode) override;
        void withDepthTesting (bool doDepthWrite) override;

        std::unique_ptr<graphics::IPipeline> build() override;
    };
}