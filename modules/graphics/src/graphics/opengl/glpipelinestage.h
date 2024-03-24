#pragma once

#include <utility>

#include "graphics/renderers/renderer_pipelinestage.h"
#include "graphics/graphics_headers.h"

#include "util/smart_help.h"

namespace phenyl::graphics {
class GLPipelineStage : public RendererPipelineStage, public util::SmartHelper<GLPipelineStage, true> {
    private:
        GLuint vaoId{};
        GLenum renderMode;
        util::Map<int, ShaderDataType> vertexAttribs{};
    public:
        explicit GLPipelineStage (PipelineStageSpec& spec);
        ~GLPipelineStage() override;

        void bindBuffer(int location, ShaderDataType attribType, const IBuffer& buffer) override;

        void render(std::size_t numVertices) override;

        //void clearBuffers() override;

        //void bufferData() override;
    };
}