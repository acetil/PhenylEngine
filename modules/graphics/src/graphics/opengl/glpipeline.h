#pragma once

#include "glframebuffer.h"
#include "util/map.h"

#include "graphics/backend/pipeline.h"
#include "graphics/graphics_headers.h"
#include "graphics/backend/shader.h"
#include "glshader.h"

namespace phenyl::graphics {
    class GlPipeline : public IPipeline {
    private:
        struct PipelineIndex {
            GLenum typeEnum;
            std::size_t typeSize;
        };

        GLuint vaoId;
        GLenum renderMode = GL_TRIANGLES;
        GlWindowFrameBuffer* windowFrameBuffer;
        core::Asset<Shader> shader;
        std::vector<std::size_t> bufferTypes;
        util::Map<UniformBinding, std::size_t> uniformTypes;
        std::optional<PipelineIndex> indexType = std::nullopt;

        bool doDepthMask = true;
        BlendMode blendMode = BlendMode::ALPHA_BLEND;
        CullMode cullMode = CullMode::NONE;

        GlShader& getShader ();
        void updateDepthMask ();
        void setBlending (const AbstractGlFrameBuffer& fb);
        void setCulling ();
        void bindFrameBuffer (IFrameBuffer* frameBuffer);
    public:
        explicit GlPipeline (GlWindowFrameBuffer* fb);
        GlPipeline (const GlPipeline&) = delete;
        GlPipeline (GlPipeline&& other) noexcept;

        GlPipeline& operator= (const GlPipeline&) = delete;
        GlPipeline& operator= (GlPipeline&& other) noexcept;

        ~GlPipeline () override;

        void bindBuffer (std::size_t type, BufferBinding binding, const IBuffer& buffer, std::size_t offset) override;
        void bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) override;
        void bindUniform (std::size_t type, UniformBinding binding, const IUniformBuffer& buffer) override;
        void bindSampler (SamplerBinding binding, const ISampler& sampler) override;
        void unbindIndexBuffer () override;

        void render (IFrameBuffer* frameBuffer, std::size_t vertices, std::size_t offset) override;
        void renderInstanced (IFrameBuffer* frameBuffer, std::size_t numInstances, std::size_t vertices, std::size_t offset) override;

        void setRenderMode (GLenum renderMode);
        void setShader (core::Asset<Shader> shader);

        BufferBinding addBuffer (std::size_t type, GLuint divisor);
        void addAttrib (GLenum type, GLint size, GLuint location, BufferBinding binding, std::size_t offset);

        UniformBinding addUniform (std::size_t type, unsigned int location);
        SamplerBinding addSampler (unsigned int location);

        void setDepthMask (bool doMask);
        void setBlendMode (BlendMode mode);
        void setCullMode (CullMode mode);
    };

    class GlPipelineBuilder : public IPipelineBuilder {
    private:
        std::unique_ptr<GlPipeline> pipeline;
    public:
        GlPipelineBuilder (GlWindowFrameBuffer* fb);

        void withGeometryType (GeometryType type) override;
        void withShader (core::Asset<Shader> shader) override;

        BufferBinding withBuffer(std::size_t type, std::size_t size, BufferInputRate inputRate) override;
        void withAttrib(ShaderDataType type, unsigned int location, BufferBinding binding, std::size_t offset) override;

        UniformBinding withUniform (std::size_t type, unsigned int location) override;
        SamplerBinding withSampler (unsigned int location) override;

        void withCullMode(CullMode mode) override;
        void withBlendMode (BlendMode mode) override;
        void withDepthMask (bool doMask) override;

        std::unique_ptr<IPipeline> build() override;
    };
}