#pragma once

#include "graphics/pipeline/pipeline_stage.h"
#include "graphics/renderers/renderer_buffer.h"

namespace graphics {
    class RendererPipelineStage {
    public:
        virtual ~RendererPipelineStage() = default;

        virtual void bindBuffer (int location, ShaderDataType attribType, std::shared_ptr<RendererBufferHandle> handle) = 0;
        virtual void render () = 0;
        virtual void clearBuffers () = 0;
        virtual void bufferData () = 0;
    };
}