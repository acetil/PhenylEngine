#pragma once

#include "graphics/abstract_render_layer.h"
#include "graphics/renderers/renderer.h"

namespace phenyl::graphics {
    class ParticleManager2D;

    class ParticleRenderLayer : public AbstractRenderLayer {
    private:
        struct Uniform {
            glm::mat4 camera;
        };

        Pipeline pipeline;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec4> colourBuffer;
        UniformBuffer<Uniform> uniformBuffer;
    public:
        ParticleRenderLayer ();

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;
        void render () override;

        void bufferData (const ParticleManager2D& manager);
    };
}