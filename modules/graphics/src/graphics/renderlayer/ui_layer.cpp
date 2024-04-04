#include "graphics/renderlayer/ui_layer.h"

#include "common/assets/assets.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 4)
#define MITER_LIMIT 2.5f

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
    uniformBuffer = renderer.makeUniformBuffer<Uniform>();

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

    // Render out in fan from start index
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

        // Add next fan triangle
        indices.emplace(startIndex);
        indices.emplace(lastIndex);
        indices.emplace(index);

        lastIndex = index;
    }
}

void UIRenderLayer::renderPolyLine (std::span<glm::vec2> points, glm::vec4 colour, float width, bool closed) {
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width / 2.0f;
    const glm::vec3 opaque = glyphAtlas.opaque();
    std::uint16_t startLIndex;
    std::uint16_t startRIndex;
    std::uint16_t lastLIndex;
    std::uint16_t lastRIndex;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto prev = i != 0 ? points[i - 1] : (closed ? points.back() : curr); // wrap around if closed, otherwise use curr
        auto next = i != points.size() - 1 ? points[i + 1] : (closed ? points.front() : curr); // wrap around if closed, otherwise use curr

        auto n1 = util::SafeNormalize(glm::vec2{-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize(glm::vec2{-(next.y - curr.y), next.x - curr.x});
        auto miter = glm::normalize(n1 + n2); // Average normals
        auto miterMult = 1.0f / glm::dot(miter, n1);
        auto miterLen = halfWidth * miterMult;

        std::uint16_t prevLIndex;
        std::uint16_t prevRIndex;
        std::uint16_t nextLIndex;
        std::uint16_t nextRIndex;
        if (miterMult <= MITER_LIMIT) {
            // Miter small enough, put in
            prevLIndex = buffer.emplace(Vertex{
                   .pos = curr - miter * miterLen,
                   .uv = opaque,
                   .colour = colour
               });
            prevRIndex = buffer.emplace(Vertex{
                .pos = curr + miter * miterLen,
                .uv = opaque,
                .colour = colour
            });

            nextLIndex = prevLIndex;
            nextRIndex = prevRIndex;
        } else {
            // Miter too large, use bevel joint
            prevLIndex = buffer.emplace(Vertex{
                .pos = curr - n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            prevRIndex = buffer.emplace(Vertex{
                .pos = curr + n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });

            nextLIndex = buffer.emplace(Vertex{
                .pos = curr - n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            nextRIndex = buffer.emplace(Vertex{
                .pos = curr + n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });

            // Add join geometry
            indices.emplace(prevLIndex);
            indices.emplace(prevRIndex);
            indices.emplace(nextLIndex);

            indices.emplace(prevRIndex);
            indices.emplace(nextLIndex);
            indices.emplace(nextRIndex);
        }

        if (i == 0) {
            // No previous vertex
            startLIndex = prevLIndex;
            startRIndex = prevRIndex;
        } else {
            // Render segment with previous vertex
            indices.emplace(lastLIndex);
            indices.emplace(lastRIndex);
            indices.emplace(prevLIndex);

            indices.emplace(lastRIndex);
            indices.emplace(prevLIndex);
            indices.emplace(prevRIndex);
        }
        lastLIndex = nextLIndex;
        lastRIndex = nextRIndex;
    }

    if (closed) {
        // Complete outline
        indices.emplace(lastLIndex);
        indices.emplace(lastRIndex);
        indices.emplace(startLIndex);

        indices.emplace(lastRIndex);
        indices.emplace(startLIndex);
        indices.emplace(startRIndex);
    }
}
