#pragma once

#include <memory>
#include <utility>

#include "common/assets/asset.h"
#include "graphics/shaders/shaders.h"
#include "graphics/renderers/buffer.h"
#include "util/meta.h"

namespace phenyl::graphics {
    class RendererPipelineStage;

    enum class PipelineType {
        TRIANGLES,
        LINES
    };

    class PipelineStage {
    private:
        std::unique_ptr<RendererPipelineStage> internal;
        common::Asset<Shader> shader;
    public:
        PipelineStage();
        explicit PipelineStage (common::Asset<Shader> _shader, std::unique_ptr<RendererPipelineStage> _internal);

        PipelineStage& operator= (PipelineStage&& other)  noexcept;
        ~PipelineStage();
        //~PipelineStage() = default;

        void bindBuffer (int location, ShaderDataType attribType, const IBuffer& buffer);

        template <typename T>
        void bindBuffer (int location, const Buffer<T>& buffer) {
            bindBuffer(location, getShaderDataType<T>(), buffer.getUnderlying());
        }

        template <typename T>
        void applyUniform (const std::string& uniformName, const T& val) {
            shader->applyUniform(uniformName, val);
        }

        void render (std::size_t numVertices);
    };

    class PipelineStageSpec {
    public:
        common::Asset<Shader> shader;
        util::Map<int, ShaderDataType> vertexAttribs;
        PipelineType type;
    };

    class PipelineStageBuilder {
    private:
        common::Asset<Shader> shader;
        util::Map<int, ShaderDataType> vertexAttribs{};
        PipelineType type = PipelineType::TRIANGLES;
    public:
        explicit PipelineStageBuilder(common::Asset<Shader> _shader) : shader{std::move(_shader)} {}

        template <typename T>
        PipelineStageBuilder& addVertexAttrib (int location) {
            vertexAttribs[location] = getShaderDataType<T>();
            return *this;
        }

        PipelineStageBuilder& withPipelineType (PipelineType type) {
            this->type = type;
            return *this;
        }

        [[nodiscard]] PipelineStageSpec build () {
            return {std::move(shader), std::move(vertexAttribs), type};
        }
    };
}