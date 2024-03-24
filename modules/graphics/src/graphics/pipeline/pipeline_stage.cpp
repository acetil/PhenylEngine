#include "graphics/pipeline/pipeline_stage.h"

#include <utility>
#include "graphics/renderers/renderer_pipelinestage.h"

using namespace phenyl::graphics;

PipelineStage::PipelineStage (common::Asset<Shader> _shader, std::unique_ptr<RendererPipelineStage> _internal) : shader{std::move(_shader)}, internal{std::move(_internal)} {

}

void PipelineStage::bindBuffer (int location, ShaderDataType attribType, const IBuffer& buffer) {
    internal->bindBuffer(location, attribType, buffer);
}

void PipelineStage::render (std::size_t numVertices) {
    shader->bind();
    internal->render(numVertices);
}

PipelineStage& PipelineStage::operator= (PipelineStage&& other) noexcept {
    internal = std::move(other.internal);
    shader = other.shader;

    return *this;
}

PipelineStage::PipelineStage () = default;

PipelineStage::~PipelineStage () = default;
