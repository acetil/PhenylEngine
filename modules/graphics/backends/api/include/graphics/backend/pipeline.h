#pragma once

#include "core/assets/asset.h"
#include "util/meta.h"

#include "buffer.h"
#include "framebuffer.h"
#include "shader.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace phenyl::graphics {
    enum class GeometryType {
        TRIANGLES,
        LINES
    };

    enum class BufferInputRate {
        VERTEX,
        INSTANCE
    };

    enum class ShaderIndexType {
        UBYTE,
        USHORT,
        UINT
    };

    enum class BlendMode {
        ALPHA_BLEND,
        ADDITIVE
    };

    enum class CullMode {
        FRONT_FACE,
        BACK_FACE,
        NONE
    };

    template <typename T>
    concept IndexType = std::unsigned_integral<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);

    template <IndexType T>
    inline constexpr ShaderIndexType GetIndexType () {
        if constexpr (sizeof(T) == 1) {
            return ShaderIndexType::UBYTE;
        } else if constexpr (sizeof(T) == 2) {
            return ShaderIndexType::USHORT;
        } else {
            return ShaderIndexType::UINT;
        }
    }

    using BufferBinding = unsigned int;
    using UniformBinding = unsigned int;
    using SamplerBinding = unsigned int;

    class IPipeline {
    public:
        virtual ~IPipeline() = default;

        virtual void bindBuffer (std::size_t type, BufferBinding binding, const IBuffer& buffer, std::size_t offset) = 0;
        virtual void bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) = 0;
        virtual void bindUniform (std::size_t type, UniformBinding binding, const IUniformBuffer& buffer, std::size_t offset, std::size_t size) = 0;
        virtual void bindSampler (SamplerBinding binding, ISampler& sampler) = 0;
        virtual void unbindIndexBuffer () = 0;
        virtual void render (IFrameBuffer* fb, std::size_t vertices, std::size_t offset) = 0; // TODO: command buffer
        virtual void renderInstanced (IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices, std::size_t offset) = 0;
    };

    class Pipeline {
    private:
        std::unique_ptr<IPipeline> pipeline;
    public:
        Pipeline () = default;
        explicit Pipeline (std::unique_ptr<IPipeline> underlying) : pipeline{std::move(underlying)} {}

        explicit operator bool () const {
            return (bool)pipeline;
        }

        Pipeline& bindBuffer (BufferBinding binding, const RawBuffer& buffer, std::size_t offset = 0) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindBuffer(0, binding, buffer.getUnderlying(), offset);

            return *this;
        }

        template <typename T>
        Pipeline& bindBuffer (BufferBinding binding, const Buffer<T>& buffer, std::size_t offset = 0) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindBuffer(meta::type_index<T>(), binding, buffer.getUnderlying(), offset * sizeof(T));

            return *this;
        }

        template <IndexType T>
        Pipeline& bindIndexBuffer (const Buffer<T>& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindIndexBuffer(GetIndexType<T>(), buffer.getUnderlying());

            return *this;
        }

        Pipeline& bindIndexBuffer (ShaderIndexType type, const RawBuffer& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindIndexBuffer(type, buffer.getUnderlying());

            return *this;
        }

        template <typename T>
        Pipeline& bindUniform (UniformBinding binding, const UniformBuffer<T>& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindUniform(meta::type_index<T>(), binding, buffer.getUnderlying(), 0, sizeof(T));

            return *this;
        }

        template <typename T>
        Pipeline& bindUniform (UniformBinding binding, const UniformArrayBuffer<T>& buffer, std::size_t index) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindUniform(meta::type_index<T>(), binding, buffer.getUnderlying(), index * buffer.stride(), sizeof(T));

            return *this;
        }

        Pipeline& bindUniform (UniformBinding binding, const RawUniformBuffer& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindUniform(0, binding, buffer.getUnderlying(), 0, buffer.size());

            return *this;
        }

        Pipeline& bindSampler (SamplerBinding binding, const Texture& texture) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindSampler(binding, texture.sampler());

            return *this;
        }

        Pipeline& bindSampler (SamplerBinding binding, ISampler& sampler) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindSampler(binding, sampler);

            return *this;
        }

        Pipeline& unbindIndexBuffer () {
            PHENYL_DASSERT(pipeline);
            pipeline->unbindIndexBuffer();

            return *this;
        }

        void render (std::size_t vertices, std::size_t offset=0) {
            PHENYL_DASSERT(pipeline);
            pipeline->render(nullptr, vertices, offset);
        }

        void render (FrameBuffer& frameBuffer, std::size_t vertices, std::size_t offset=0) {
            PHENYL_DASSERT(pipeline);
            pipeline->render(&frameBuffer.getUnderlying(), vertices, offset);
        }

        void renderInstanced (std::size_t numInstances, std::size_t vertices, std::size_t offset=0) {
            PHENYL_DASSERT(pipeline);
            pipeline->renderInstanced(nullptr, numInstances, vertices, offset);
        }

        void renderInstanced (FrameBuffer& frameBuffer, std::size_t numInstances, std::size_t vertices, std::size_t offset=0) {
            PHENYL_DASSERT(pipeline);
            pipeline->renderInstanced(&frameBuffer.getUnderlying(), numInstances, vertices, offset);
        }
    };

    class IPipelineBuilder {
    public:
        virtual ~IPipelineBuilder() = default;

        virtual void withBlendMode (BlendMode mode) = 0;
        virtual void withDepthTesting (bool doDepthWrite) = 0;
        virtual void withCullMode (CullMode mode) = 0;
        virtual void withGeometryType (GeometryType type) = 0;
        virtual void withShader (core::Asset<Shader> shader) = 0;

        virtual BufferBinding withBuffer (std::size_t type, std::size_t size, BufferInputRate inputRate) = 0;
        virtual void withAttrib (ShaderDataType type, unsigned int location, BufferBinding binding, std::size_t offset) = 0;

        virtual UniformBinding withUniform (std::size_t type, unsigned int location) = 0;
        virtual SamplerBinding withSampler (unsigned int location) = 0;

        virtual std::unique_ptr<IPipeline> build () = 0;
    };

    class PipelineBuilder {
    private:
        std::unique_ptr<IPipelineBuilder> builder;
    public:
        explicit PipelineBuilder (std::unique_ptr<IPipelineBuilder> builder) : builder{std::move(builder)} {}

        PipelineBuilder& withGeometryType (GeometryType type) {
            PHENYL_DASSERT(builder);
            builder->withGeometryType(type);

            return *this;
        }

        PipelineBuilder& withShader (core::Asset<Shader> shader) {
            PHENYL_DASSERT(builder);
            builder->withShader(std::move(shader));

            return *this;
        }

        PipelineBuilder& withRawBuffer (BufferBinding& bindingOut, std::size_t stride, BufferInputRate inputRate = BufferInputRate::VERTEX) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withBuffer(0, stride, inputRate);

            return *this;
        }

        template <typename T>
        PipelineBuilder& withBuffer (BufferBinding& bindingOut, BufferInputRate inputRate = BufferInputRate::VERTEX) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withBuffer(meta::type_index<T>(), sizeof(T), inputRate);

            return *this;
        }

        PipelineBuilder& withAttrib (unsigned int location, BufferBinding binding, ShaderDataType type, std::size_t offset = 0) {
            PHENYL_DASSERT(builder);
            builder->withAttrib(type, location, binding, offset);

            return *this;
        }

        template <typename T>
        PipelineBuilder& withAttrib (unsigned int location, BufferBinding binding, std::size_t offset = 0) {
            PHENYL_DASSERT(builder);
            builder->withAttrib(GetShaderDataType<T>(), location, binding, offset);

            return *this;
        }

        template <typename T>
        PipelineBuilder& withUniform (unsigned int location, UniformBinding& bindingOut) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withUniform(meta::type_index<T>(), location);

            return *this;
        }

        PipelineBuilder& withRawUniform (unsigned int location, UniformBinding& bindingOut) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withUniform(0, location);

            return *this;
        }

        PipelineBuilder& withSampler2D (unsigned int location, SamplerBinding& bindingOut) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withSampler(location);

            return *this;
        }

        PipelineBuilder& withBlending (BlendMode mode) {
            PHENYL_DASSERT(builder);
            builder->withBlendMode(mode);

            return *this;
        }

        PipelineBuilder& withCulling (CullMode mode) {
            PHENYL_DASSERT(builder);
            builder->withCullMode(mode);

            return *this;
        }

        PipelineBuilder& withDepthTesting (bool doWrite = true) {
            PHENYL_DASSERT(builder);
            builder->withDepthTesting(doWrite);

            return *this;
        }

        Pipeline build () {
            PHENYL_DASSERT(builder);

            return Pipeline{builder->build()};
        }
    };
}