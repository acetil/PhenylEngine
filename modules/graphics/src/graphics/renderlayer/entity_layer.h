#pragma once

#include "graphics/abstract_render_layer.h"
#include "graphics/camera.h"
#include "component/component.h"
#include "graphics/textures/sprite_atlas.h"

namespace phenyl::graphics {
    class EntityRenderLayer : public AbstractRenderLayer {
    private:
        struct Uniform {
            glm::mat4 camera;
        };

        Pipeline pipeline;

        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec2> uvBuffer;

        UniformBinding uniformBinding{};
        UniformBuffer<Uniform> uniformBuffer;

        SamplerBinding samplerBinding{};
        std::unique_ptr<SpriteAtlas> atlas;

        void bufferData (const component::ComponentManager& manager, const Camera& camera);
    public:
        EntityRenderLayer ();

        [[nodiscard]] std::string_view getName () const override;

        void init (Renderer& renderer) override;

        void preRender (component::ComponentManager& manager, const Camera& camera);
        void render () override;
    };
}
