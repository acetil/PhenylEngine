#pragma once

#include "common/assets/asset.h"
#include "util/meta.h"

#include "graphics/shaders/shaders.h"
#include "graphics/renderers/buffer.h"
#include "graphics/renderers/uniform_buffer.h"

namespace phenyl::graphics {
    class GraphicsTexture;

    enum class GeometryType {
        TRIANGLES,
        LINES
    };

    enum class BufferInputRate {
        VERTEX,
        INSTANCE
    };


    using BufferBinding = unsigned int;
    using UniformBinding = unsigned int;
    using SamplerBinding = unsigned int;

    class IPipeline {
    public:
        virtual ~IPipeline() = default;

        virtual void bindBuffer (std::size_t type, BufferBinding binding, IBuffer& buffer) = 0;
        virtual void bindUniform (std::size_t type, UniformBinding binding, IUniformBuffer& buffer) = 0;
        virtual void bindSampler (SamplerBinding binding, GraphicsTexture& texture) = 0; // TODO
        virtual void render (std::size_t vertices) = 0; // TODO: command buffer
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

        template <typename T>
        Pipeline& bindBuffer (BufferBinding binding, Buffer<T>& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindBuffer(meta::type_index<T>(), binding, buffer.getUnderlying());

            return *this;
        }

        template <typename T>
        Pipeline& bindUniform (UniformBinding binding, UniformBuffer<T>& buffer) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindUniform(meta::type_index<T>(), binding, buffer.getUnderlying());

            return *this;
        }

        Pipeline& bindSampler (SamplerBinding binding, GraphicsTexture& texture) {
            PHENYL_DASSERT(pipeline);
            pipeline->bindSampler(binding, texture);

            return *this;
        }

        void render (std::size_t vertices) {
            PHENYL_DASSERT(pipeline);
            pipeline->render(vertices);
        }
    };

    class IPipelineBuilder {
    public:
        virtual ~IPipelineBuilder() = default;

        virtual void withGeometryType (GeometryType type) = 0;
        virtual void withShader (common::Asset<Shader> shader) = 0;

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

        PipelineBuilder& withShader (common::Asset<Shader> shader) {
            PHENYL_DASSERT(builder);
            builder->withShader(std::move(shader));

            return *this;
        }

        template <typename T>
        PipelineBuilder& withBuffer (BufferBinding& bindingOut, BufferInputRate inputRate = BufferInputRate::VERTEX) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withBuffer(meta::type_index<T>(), sizeof(T), inputRate);

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

        PipelineBuilder& withSampler2D (unsigned int location, SamplerBinding& bindingOut) {
            PHENYL_DASSERT(builder);
            bindingOut = builder->withSampler(location);

            return *this;
        }

        Pipeline build () {
            PHENYL_DASSERT(builder);

            return Pipeline{builder->build()};
        }
    };
}