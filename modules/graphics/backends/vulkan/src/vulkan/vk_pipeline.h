#pragma once

#include "graphics/backend/pipeline.h"

namespace phenyl::vulkan {
    class VulkanPipeline : public graphics::IPipeline {
    public:
        void bindBuffer (std::size_t type, graphics::BufferBinding binding, const graphics::IBuffer& buffer, std::size_t offset) override;
        void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
        void bindUniform (std::size_t type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer) override;
        void bindSampler (graphics::SamplerBinding binding, const graphics::ISampler& sampler) override;

        void unbindIndexBuffer () override;

        void render (graphics::IFrameBuffer* fb, std::size_t vertices, std::size_t offset) override;
        void renderInstanced (graphics::IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices, std::size_t offset) override;
    };

    class VulkanPipelineBuilder : public graphics::IPipelineBuilder {
    public:
        void withBlendMode (graphics::BlendMode mode) override;
        void withCullMode (graphics::CullMode mode) override;
        void withDepthMask (bool doMask) override;
        void withGeometryType (graphics::GeometryType type) override;

        void withShader (core::Asset<graphics::Shader> shader) override;

        graphics::BufferBinding withBuffer (std::size_t type, std::size_t size, graphics::BufferInputRate inputRate) override;
        void withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding, std::size_t offset) override;

        graphics::UniformBinding withUniform (std::size_t type, unsigned int location) override;
        graphics::SamplerBinding withSampler (unsigned int location) override;

        std::unique_ptr<graphics::IPipeline> build () override;
    };
}