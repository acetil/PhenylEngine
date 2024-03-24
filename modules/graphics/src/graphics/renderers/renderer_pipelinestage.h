#pragma once

#include "graphics/pipeline/pipeline_stage.h"

namespace phenyl::graphics {
    class RendererPipelineStage {
    public:
        virtual ~RendererPipelineStage() = default;

        virtual void bindBuffer (int location, ShaderDataType attribType, const IBuffer& buffer) = 0;
        virtual void render (std::size_t numVertices) = 0;
        //virtual void clearBuffers () = 0;
        //virtual void bufferData () = 0;
    };
}