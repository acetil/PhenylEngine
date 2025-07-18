#pragma once

#include "glframebuffer.h"
#include "glshader.h"
#include "graphics/backend/pipeline.h"
#include "graphics/backend/shader.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
class GlPipeline : public graphics::IPipeline {
public:
    explicit GlPipeline (GlWindowFrameBuffer* fb);
    GlPipeline (const GlPipeline&) = delete;
    GlPipeline (GlPipeline&& other) noexcept;

    GlPipeline& operator= (const GlPipeline&) = delete;
    GlPipeline& operator= (GlPipeline&& other) noexcept;

    ~GlPipeline () override;

    void bindBuffer (meta::TypeIndex type, graphics::BufferBinding binding, const graphics::IBuffer& buffer,
        std::size_t offset) override;
    void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
    void bindUniform (meta::TypeIndex, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer,
        std::size_t offset, std::size_t size) override;
    void bindSampler (graphics::SamplerBinding binding, graphics::ISampler& sampler) override;
    void unbindIndexBuffer () override;

    void render (graphics::ICommandList& list, graphics::IFrameBuffer* frameBuffer, std::size_t vertices,
        std::size_t offset) override;
    void renderInstanced (graphics::ICommandList& list, graphics::IFrameBuffer* frameBuffer, std::size_t numInstances,
        std::size_t vertices, std::size_t offset) override;

    void setRenderMode (GLenum renderMode);
    void setShader (std::shared_ptr<graphics::Shader> shader);

    graphics::BufferBinding addBuffer (meta::TypeIndex type, GLuint divisor);
    void addAttrib (GLenum type, GLint size, GLuint location, graphics::BufferBinding binding, std::size_t offset);

    graphics::UniformBinding addUniform (meta::TypeIndex type, unsigned int location);
    graphics::SamplerBinding addSampler (unsigned int location);

    void setDepthTest (bool doMask);
    void setBlendMode (graphics::BlendMode mode);
    void setCullMode (graphics::CullMode mode);

private:
    struct PipelineIndex {
        GLenum typeEnum;
        std::size_t typeSize;
    };

    GLuint m_vao;
    GLenum m_renderMode = GL_TRIANGLES;
    GlWindowFrameBuffer* m_windowFrameBuffer;
    std::shared_ptr<graphics::Shader> m_shader;
    std::vector<meta::TypeIndex> m_bufferTypes;
    std::unordered_map<graphics::UniformBinding, meta::TypeIndex> m_uniformTypes;
    std::optional<PipelineIndex> m_indexType = std::nullopt;

    bool m_doDepthTest = false;
    bool m_doDepthMask = false;
    graphics::BlendMode m_blendMode = graphics::BlendMode::ALPHA_BLEND;
    graphics::CullMode m_cullMode = graphics::CullMode::NONE;

    GlShader& getShader ();
    void updateDepthMask ();
    void setBlending (const AbstractGlFrameBuffer& fb);
    void setCulling ();
    void bindFrameBuffer (graphics::IFrameBuffer* frameBuffer);
};

class GlPipelineBuilder : public graphics::IPipelineBuilder {
public:
    GlPipelineBuilder (GlWindowFrameBuffer* fb);

    void withGeometryType (graphics::GeometryType type) override;
    void withShader (const std::shared_ptr<graphics::Shader>& shader) override;

    graphics::BufferBinding withBuffer (meta::TypeIndex type, std::size_t size,
        graphics::BufferInputRate inputRate) override;
    void withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding,
        std::size_t offset) override;

    graphics::UniformBinding withUniform (meta::TypeIndex type, unsigned int location) override;
    graphics::SamplerBinding withSampler (unsigned int location) override;

    void withCullMode (graphics::CullMode mode) override;
    void withBlendMode (graphics::BlendMode mode) override;
    void withDepthTesting (bool doDepthWrite) override;

    std::unique_ptr<graphics::IPipeline> build () override;

private:
    std::unique_ptr<GlPipeline> m_pipeline;
};
} // namespace phenyl::opengl
