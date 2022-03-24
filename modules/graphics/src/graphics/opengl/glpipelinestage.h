#pragma once

#include <utility>

#include "graphics/renderers/renderer_pipelinestage.h"
#include "graphics/graphics_headers.h"
#include "graphics/opengl/glbuffer.h"

#include "util/smart_help.h"

namespace graphics {
class GLPipelineStage : public RendererPipelineStage, public util::SmartHelper<GLPipelineStage, true> {
    private:
        GLuint vaoId{};
        util::Map<int, std::pair<ShaderDataType, std::shared_ptr<GlBuffer>>> vertexAttribs{};
    public:
        explicit GLPipelineStage (PipelineStageSpec& spec);
        ~GLPipelineStage() override;

        void bindBuffer(int location, ShaderDataType attribType, std::shared_ptr<RendererBufferHandle> handle) override;

        void render() override;

        void clearBuffers() override;

        void bufferData() override;
    };
}