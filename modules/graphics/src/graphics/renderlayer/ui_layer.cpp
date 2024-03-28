#include "graphics/renderlayer/ui_layer.h"

#include "graphics/font/font.h"
#include "graphics/font/rendered_text.h"
#include "common/assets/assets.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 6)

UIRenderLayer::UIRenderLayer (const Texture& fontTexture) : AbstractRenderLayer{4}, fontTexture{fontTexture} {}

std::string_view UIRenderLayer::getName () const {
    return "UIRenderLayer";
}

void UIRenderLayer::init (Renderer& renderer) {
    BufferBinding textBinding;
    textPipeline = renderer.buildPipeline()
                           .withShader(phenyl::common::Assets::Load<Shader>("phenyl/shaders/text"))
                           .withBuffer<TextVertex>(textBinding)
                           .withAttrib<glm::vec2>(0, textBinding, offsetof(TextVertex, pos))
                           .withAttrib<glm::vec2>(1, textBinding, offsetof(TextVertex, uv))
                           .withAttrib<glm::vec3>(2, textBinding, offsetof(TextVertex, colour))
                           .withSampler2D(0, samplerBinding)
                           .build();


    textBuffer = renderer.makeBuffer<TextVertex>(BUFFER_SIZE);

    textPipeline.bindBuffer(textBinding, textBuffer);
    textPipeline.bindSampler(samplerBinding, fontTexture);

    BufferBinding boxBinding;
    auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/box");
    boxPipeline = renderer.buildPipeline()
                          .withShader(shader)
                          .withBuffer<BoxVertex>(boxBinding)
                          .withAttrib<glm::vec2>(0, boxBinding, offsetof(BoxVertex, pos))
                          .withAttrib<glm::vec2>(1, boxBinding, offsetof(BoxVertex, rectPos))
                          .withAttrib<glm::vec4>(2, boxBinding, offsetof(BoxVertex, borderColour))
                          .withAttrib<glm::vec4>(3, boxBinding, offsetof(BoxVertex, bgColour))
                          .withAttrib<glm::vec4>(4, boxBinding, offsetof(BoxVertex, details))
                          .withUniform<Uniform>(shader->getUniformLocation("Uniform"), uniformBinding)
                          .build();

    boxBuffer = renderer.makeBuffer<BoxVertex>(BUFFER_SIZE);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>(glm::vec2{800, 600});

    boxPipeline.bindBuffer(boxBinding, boxBuffer);
    boxPipeline.bindUniform(uniformBinding, uniformBuffer);
}

void UIRenderLayer::bufferText (const RenderedText& text) {
    auto posIt = text.getPosComp().cbegin();
    auto uvIt = text.getUvComp().cbegin();
    auto colourIt = text.getColourComp().cbegin();

    while (posIt != text.getPosComp().cend()) {
        textBuffer.emplace(TextVertex{
                .pos = *posIt,
                .uv = *uvIt,
                .colour = *colourIt
        });

        ++posIt;
        ++uvIt;
        ++colourIt;
    }
}

void UIRenderLayer::bufferRect (const UIRect& rect) {
    for (int i = 0; i < 6; i++) {
        int vertexNum = i == 3 ? i : i % 3;

        glm::vec2 pos{rect.screenPos.x + (float)(vertexNum % 2 == 1) * rect.size.x, rect.screenPos.y + (float)(vertexNum / 2 == 1) * rect.size.y};
        glm::vec2 rectPos{-1.0f + (float)(vertexNum % 2 == 1) * 2.0f, -1.0f + (float)(vertexNum / 2 == 1) * 2.0f};

        boxBuffer.emplace(BoxVertex{
                .pos = pos,
                .rectPos = rectPos,
                .borderColour = rect.borderColour,
                .bgColour = rect.bgColour,
                .details = rect.details
        });
    }
}

void UIRenderLayer::uploadData () {
    textBuffer.upload();
    boxBuffer.upload();
}

void UIRenderLayer::setScreenSize (glm::vec2 screenSize) {
    uniformBuffer->screenSize = screenSize;
}

void UIRenderLayer::render () {
    textPipeline.bindSampler(samplerBinding, fontTexture);
    textPipeline.render(textBuffer.size());

    boxPipeline.bindUniform(uniformBinding, uniformBuffer);
    boxPipeline.render(boxBuffer.size());

    textBuffer.clear();
    boxBuffer.clear();
}
