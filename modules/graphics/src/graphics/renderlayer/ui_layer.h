#pragma once

#include "graphics/abstract_render_layer.h"
#include "graphics/renderers/renderer.h"
#include "graphics/maths_headers.h"
#include "graphics/ui/ui_rect.h"
#include "graphics/font/rendered_text.h"

namespace phenyl::graphics {
    class UIRenderLayer : public AbstractRenderLayer {
    private:
        struct Uniform {
            glm::vec2 screenSize;
        };

        struct TextVertex {
            glm::vec2 pos;
            glm::vec2 uv;
            glm::vec3 colour;
        };

        struct BoxVertex {
            glm::vec2 pos;
            glm::vec2 rectPos;
            glm::vec4 borderColour;
            glm::vec4 bgColour;
            glm::vec4 details;
        };

        Pipeline textPipeline;
        GraphicsTexture fontTexture;
        SamplerBinding samplerBinding{};
        Buffer<TextVertex> textBuffer;

        Pipeline boxPipeline;
        Buffer<BoxVertex> boxBuffer;

        UniformBinding uniformBinding{};
        UniformBuffer<Uniform> uniformBuffer;
    public:
        explicit UIRenderLayer (GraphicsTexture fontTexture);

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;
        void render () override;

        void bufferText (const RenderedText& text);
        void bufferRect (const UIRect& rect);

        void uploadData ();

        void setScreenSize (glm::vec2 screenSize);
    };
}
