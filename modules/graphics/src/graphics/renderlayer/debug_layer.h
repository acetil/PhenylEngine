#pragma once

#include "graphics/backend/abstract_render_layer.h"
#include "graphics/camera_2d.h"
#include "graphics/backend/renderer.h"

namespace phenyl::graphics {
    struct DebugBox;
    struct DebugLine;

    class DebugLayer : public AbstractRenderLayer {
    public:
        DebugLayer ();

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;

        void bufferData (const Camera2D& camera, glm::vec2 screenSize);

        void render () override;

    private:
        struct Uniform {
            glm::mat4 camera;
            glm::mat4 screenTransform;
        };

        Pipeline m_boxPipeline;
        Buffer<glm::vec3> m_boxPos;
        Buffer<glm::vec4> m_boxColor;

        Pipeline m_linePipeline;
        Buffer<glm::vec3> m_linePos;
        Buffer<glm::vec4> m_lineColor;

        UniformBuffer<Uniform> m_uniformBuffer;

        void bufferBox (const DebugBox& box);
        void bufferLine (const DebugLine& line);
    };
}