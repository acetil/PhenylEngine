#pragma once

#include "graphics/renderers/renderer.h"
#include "graphics/camera.h"
#include "graphics/abstract_render_layer.h"

namespace phenyl::graphics {
    struct DebugBox;
    struct DebugLine;

    class DebugLayer : public AbstractRenderLayer {
    private:
        struct Uniform {
            glm::mat4 camera;
            glm::mat4 screenTransform;
        };

        Pipeline boxPipeline;
        Buffer<glm::vec3> boxPos;
        Buffer<glm::vec4> boxColour;

        Pipeline linePipeline;
        Buffer<glm::vec3> linePos;
        Buffer<glm::vec4> lineColour;

        UniformBuffer<Uniform> uniformBuffer;

        void bufferBox (const DebugBox& box);
        void bufferLine (const DebugLine& line);
    public:
        DebugLayer ();

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;

        void bufferData (const Camera& camera, glm::vec2 screenSize);

        void render () override;
    };
}