#include "graphics/pipeline/pipeline_stage.h"
#include "graphics/renderers/renderer_pipelinestage.h"

using namespace graphics;

PipelineStage::PipelineStage (const ShaderProgramNew& _shader, std::unique_ptr<RendererPipelineStage> _internal) : shader{_shader}, internal{std::move(_internal)} {

}


void PipelineStage::bindBuffer (int location, ShaderDataType attribType, const std::shared_ptr<RendererBufferHandle>& handle) {
    internal->bindBuffer(location, attribType, handle);
}

void PipelineStage::render () {
    shader.bind();
    internal->render();
}

PipelineStage& PipelineStage::operator= (PipelineStage&& other) noexcept {
    internal = std::move(other.internal);
    shader = other.shader;

    return *this;
}

void PipelineStage::clearBuffers () {
    internal->clearBuffers();
}

void PipelineStage::bufferAllData () {
    internal->bufferData();
}

PipelineStage::PipelineStage () = default;

PipelineStage::~PipelineStage () = default;
