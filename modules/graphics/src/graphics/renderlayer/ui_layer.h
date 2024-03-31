#pragma once

#include "graphics/abstract_render_layer.h"
#include "graphics/renderers/renderer.h"
#include "graphics/maths_headers.h"
#include "graphics/ui/ui_rect.h"
#include "graphics/font/font.h"
#include "graphics/font/font_manager.h"

namespace phenyl::graphics {
    class UIRenderLayer : public AbstractRenderLayer, public IGlyphRenderer{
    private:
        struct Uniform {
            glm::vec2 screenSize;
        };

        struct BoxVertex {
            glm::vec2 pos;
            glm::vec2 rectPos;
            glm::vec4 borderColour;
            glm::vec4 bgColour;
            glm::vec4 details;
        };

        Pipeline textPipeline;
        GlyphAtlas& glyphAtlas;
        SamplerBinding samplerBinding{};
        Buffer<TextVertex> textBuffer;
        UniformBinding textUniformBinding;
        //Buffer<TextVertex> textBuffer;

        Pipeline boxPipeline;
        Buffer<BoxVertex> boxBuffer;

        UniformBinding uniformBinding{};
        UniformBuffer<Uniform> uniformBuffer;
    public:
        explicit UIRenderLayer (GlyphAtlas& glyphAtlas);

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;
        void render () override;

        void bufferRect (const UIRect& rect);

        void uploadData ();

        void setScreenSize (glm::vec2 screenSize);

        Buffer<TextVertex>& getTextBuffer ();
        void renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) override;
    };
}
