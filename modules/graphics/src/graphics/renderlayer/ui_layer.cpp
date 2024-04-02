#include "graphics/renderlayer/ui_layer.h"

#include "common/assets/assets.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 4)

UIRenderLayer::UIRenderLayer (GlyphAtlas& glyphAtlas) : AbstractRenderLayer{4}, glyphAtlas{glyphAtlas} {}

std::string_view UIRenderLayer::getName () const {
    return "UIRenderLayer";
}

void UIRenderLayer::init (Renderer& renderer) {
    BufferBinding textBinding;
    auto textShader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/text");
    textPipeline = renderer.buildPipeline()
                           .withShader(textShader)
                           .withBuffer<TextVertex>(textBinding)
                           .withAttrib<glm::vec2>(0, textBinding, offsetof(TextVertex, pos))
                           .withAttrib<glm::vec3>(1, textBinding, offsetof(TextVertex, uv))
                           .withAttrib<glm::vec3>(2, textBinding, offsetof(TextVertex, colour))
                           .withSampler2D(*textShader->samplerLocation("textureSampler"), samplerBinding)
                           .withUniform<Uniform>(*textShader->uniformLocation("Uniform"), textUniformBinding)
                           .build();

    textBuffer = renderer.makeBuffer<TextVertex>(BUFFER_SIZE);
    textIndices = renderer.makeBuffer<std::uint16_t>(BUFFER_SIZE);

    textPipeline.bindBuffer(textBinding, textBuffer);
    textPipeline.bindIndexBuffer(textIndices);
    textPipeline.bindSampler(samplerBinding, glyphAtlas.sampler());

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
                          .withUniform<Uniform>(*shader->uniformLocation("Uniform"), uniformBinding)
                          .build();

    boxBuffer = renderer.makeBuffer<BoxVertex>(BUFFER_SIZE);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>(glm::vec2{800, 600});

    boxPipeline.bindBuffer(boxBinding, boxBuffer);
    boxPipeline.bindUniform(uniformBinding, uniformBuffer);
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
    textIndices.upload();
    boxBuffer.upload();
    glyphAtlas.upload();
}

void UIRenderLayer::setScreenSize (glm::vec2 screenSize) {
    uniformBuffer->screenSize = screenSize;
}

void UIRenderLayer::render () {
    textPipeline.bindSampler(samplerBinding, glyphAtlas.sampler());
    textPipeline.bindUniform(textUniformBinding, uniformBuffer);
    textPipeline.render(textIndices.size());

    boxPipeline.bindUniform(uniformBinding, uniformBuffer);
    boxPipeline.render(boxBuffer.size());

    textBuffer.clear();
    textIndices.clear();
    boxBuffer.clear();
}

Buffer<TextVertex>& UIRenderLayer::getTextBuffer () {
    return textBuffer;
}

void UIRenderLayer::renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) {
    auto topLeft = pos;
    auto bottomRight = pos + glyph.size;

    auto startIndex = textBuffer.emplace(TextVertex{
            .pos = topLeft,
            .uv = glm::vec3{glyph.uvStart, glyph.atlasLayer},
            .colour = colour
    });
    textBuffer.emplace(TextVertex{
            .pos = glm::vec2{bottomRight.x, topLeft.y},
            .uv = glm::vec3{glyph.uvEnd.x, glyph.uvStart.y, glyph.atlasLayer},
            .colour = colour
    });
    textBuffer.emplace(TextVertex{
            .pos = glm::vec2{topLeft.x, bottomRight.y},
            .uv = glm::vec3{glyph.uvStart.x, glyph.uvEnd.y, glyph.atlasLayer},
            .colour = colour
    });
    textBuffer.emplace(TextVertex{
            .pos = bottomRight,
            .uv = glm::vec3{glyph.uvEnd, glyph.atlasLayer},
            .colour = colour
    });

    textIndices.emplace(startIndex + 0);
    textIndices.emplace(startIndex + 1);
    textIndices.emplace(startIndex + 2);

    textIndices.emplace(startIndex + 1);
    textIndices.emplace(startIndex + 2);
    textIndices.emplace(startIndex + 3);
}
