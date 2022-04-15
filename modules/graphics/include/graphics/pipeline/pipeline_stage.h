#pragma once

#include <memory>

#include "graphics/shaders/shader_new.h"
#include "graphics/renderers/buffer.h"
#include "util/meta.h"

namespace graphics {
    class RendererPipelineStage;

    class PipelineStage {
    private:
        std::unique_ptr<RendererPipelineStage> internal;
        ShaderProgramNew shader;
    public:
        PipelineStage();
        explicit PipelineStage (const ShaderProgramNew& _shader, std::unique_ptr<RendererPipelineStage> _internal);

        PipelineStage& operator= (PipelineStage&& other)  noexcept;
        ~PipelineStage();
        //~PipelineStage() = default;

        void bindBuffer (int location, ShaderDataType attribType, const std::shared_ptr<RendererBufferHandle>& handle);

        template <typename T>
        void bindBuffer (int location, Buffer<T> buffer) {
            bindBuffer(location, getShaderDataType<T>(), buffer.bufferHandle);
        }

        template <typename T>
        void applyUniform (const std::string& uniformName, const T& val) {
            shader.applyUniform(uniformName, val);
        }

        void render ();

        void clearBuffers ();

        void bufferAllData ();
    };

    class PipelineStageSpec {
    public:
        ShaderProgramNew shader;
        util::Map<int, ShaderDataType> vertexAttribs;
    };

    class PipelineStageBuilder {
    private:
        ShaderProgramNew shader;
        util::Map<int, ShaderDataType> vertexAttribs{};
    public:
        explicit PipelineStageBuilder(const ShaderProgramNew& _shader) : shader{_shader} {}

        template <typename T>
        PipelineStageBuilder& addVertexAttrib (int location) {
            vertexAttribs[location] = getShaderDataType<T>();
            return *this;
        }

        [[nodiscard]] PipelineStageSpec build () const {
            return {shader, vertexAttribs};
        }
    };
}