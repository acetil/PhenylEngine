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
    pipeline = renderer.buildPipeline()
                       .withShader(textShader)
                       .withBuffer<Vertex>(textBinding)
                       .withAttrib<glm::vec2>(0, textBinding, offsetof(Vertex, pos))
                       .withAttrib<glm::vec3>(1, textBinding, offsetof(Vertex, uv))
                       .withAttrib<glm::vec3>(2, textBinding, offsetof(Vertex, colour))
                       .withSampler2D(*textShader->samplerLocation("textureSampler"), samplerBinding)
                       .withUniform<Uniform>(*textShader->uniformLocation("Uniform"), uniformBinding)
                       .build();

    buffer = renderer.makeBuffer<Vertex>(BUFFER_SIZE);
    indices = renderer.makeBuffer<std::uint16_t>(BUFFER_SIZE);

    pipeline.bindBuffer(textBinding, buffer);
    pipeline.bindIndexBuffer(indices);
    pipeline.bindSampler(samplerBinding, glyphAtlas.sampler());
}

void UIRenderLayer::uploadData () {
    buffer.upload();
    indices.upload();
    glyphAtlas.upload();
}

void UIRenderLayer::setScreenSize (glm::vec2 screenSize) {
    uniformBuffer->screenSize = screenSize;
}

void UIRenderLayer::render () {
    pipeline.bindSampler(samplerBinding, glyphAtlas.sampler());
    pipeline.bindUniform(uniformBinding, uniformBuffer);
    pipeline.render(indices.size());

    buffer.clear();
    indices.clear();
}

void UIRenderLayer::renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) {
    auto topLeft = pos;
    auto bottomRight = pos + glyph.size;

    auto startIndex = buffer.emplace(Vertex{
            .pos = topLeft,
            .uv = glm::vec3{glyph.uvStart, glyph.atlasLayer},
            .colour = glm::vec4{colour, 1.0f}
    });
    buffer.emplace(Vertex{
            .pos = glm::vec2{bottomRight.x, topLeft.y},
            .uv = glm::vec3{glyph.uvEnd.x, glyph.uvStart.y, glyph.atlasLayer},
            .colour = glm::vec4{colour, 1.0f}
    });
    buffer.emplace(Vertex{
            .pos = glm::vec2{topLeft.x, bottomRight.y},
            .uv = glm::vec3{glyph.uvStart.x, glyph.uvEnd.y, glyph.atlasLayer},
            .colour = glm::vec4{colour, 1.0f}
    });
    buffer.emplace(Vertex{
            .pos = bottomRight,
            .uv = glm::vec3{glyph.uvEnd, glyph.atlasLayer},
            .colour = glm::vec4{colour, 1.0f}
    });

    indices.emplace(startIndex + 0);
    indices.emplace(startIndex + 1);
    indices.emplace(startIndex + 2);

    indices.emplace(startIndex + 1);
    indices.emplace(startIndex + 2);
    indices.emplace(startIndex + 3);
}

void UIRenderLayer::renderConvexPoly (std::span<glm::vec2> points, glm::vec4 colour) {
    if (points.size() < 3 || colour.a <= 0.0f) {
        return;
    }

    const glm::vec3 opaque = glyphAtlas.opaque();
    std::uint16_t startIndex = buffer.emplace(Vertex{
        .pos = points[0],
        .uv = opaque,
        .colour = colour
    });
    std::uint16_t lastIndex = buffer.emplace(Vertex{
        .pos = points[1],
        .uv = opaque,
        .colour = colour
    });

    for (std::size_t i = 2; i < points.size(); i++) {
        std::uint16_t index = buffer.emplace(Vertex{
            .pos = points[i],
            .uv = opaque,
            .colour = colour
        });

        indices.emplace(startIndex);
        indices.emplace(lastIndex);
        indices.emplace(index);

        lastIndex = index;
    }
}

void UIRenderLayer::renderPolyLine (std::span<glm::vec2> points, glm::vec4 colour, float width) {
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width / 2.0f;
    const glm::vec3 opaque = glyphAtlas.opaque();
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto next = points[(i + 1) % points.size()];
        if (curr == next) {
            continue;
        }

        auto norm = glm::normalize(glm::vec2{-(next.y - curr.y), next.x - curr.x});

        auto startIndex = buffer.emplace(Vertex{
            .pos = curr - norm * halfWidth,
            .uv = opaque,
            .colour = colour
        });
        buffer.emplace(Vertex{
                .pos = curr + norm * halfWidth,
                .uv = opaque,
                .colour = colour
        });
        buffer.emplace(Vertex{
                .pos = next - norm * halfWidth,
                .uv = opaque,
                .colour = colour
        });
        buffer.emplace(Vertex{
                .pos = next + norm * halfWidth,
                .uv = opaque,
                .colour = colour
        });

        indices.emplace(startIndex + 0);
        indices.emplace(startIndex + 1);
        indices.emplace(startIndex + 2);

        indices.emplace(startIndex + 1);
        indices.emplace(startIndex + 2);
        indices.emplace(startIndex + 3);
    }
}
