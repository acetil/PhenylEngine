#pragma once

#include "graphics/backend/abstract_render_layer.h"
#include "graphics/backend/renderer.h"
#include "graphics/camera_2d.h"

namespace phenyl::graphics {
class ParticleManager2D;

class ParticleRenderLayer : public AbstractRenderLayer {
public:
    ParticleRenderLayer ();

    [[nodiscard]] std::string_view getName () const override;
    void init (Renderer& renderer) override;
    void render () override;

    void bufferData (const Camera2D& camera, const ParticleManager2D& manager);

private:
    struct Uniform {
        glm::mat4 camera;
    };

    Pipeline m_pipeline;
    Buffer<glm::vec2> m_posBuffer;
    Buffer<glm::vec4> m_colorBuffer;
    UniformBuffer<Uniform> m_uniformBuffer;
};
} // namespace phenyl::graphics
