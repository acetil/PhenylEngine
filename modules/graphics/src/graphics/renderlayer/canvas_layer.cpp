#include "graphics/renderlayer/canvas_layer.h"
#include "graphics/font/glyph_atlas.h"

#include "common/assets/assets.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 4)
#define MITER_LIMIT 2.5f

CanvasRenderLayer::CanvasRenderLayer (GlyphAtlas& glyphAtlas) : AbstractRenderLayer{4}, glyphAtlas{glyphAtlas} {}

std::string_view CanvasRenderLayer::getName () const {
    return "CanvasRenderLayer";
}

void CanvasRenderLayer::init (Renderer& renderer) {
    BufferBinding textBinding;
    auto textShader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/canvas");
    pipeline = renderer.buildPipeline()
                       .withShader(textShader)
                       .withBuffer<Vertex>(textBinding)
                       .withAttrib<glm::vec2>(0, textBinding, offsetof(Vertex, pos))
                       .withAttrib<glm::vec3>(1, textBinding, offsetof(Vertex, uv))
                       .withAttrib<glm::vec4>(2, textBinding, offsetof(Vertex, colour))
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

void CanvasRenderLayer::uploadData () {
    buffer.upload();
    indices.upload();
    glyphAtlas.upload();
}

void CanvasRenderLayer::setScreenSize (glm::vec2 screenSize) {
    uniformBuffer->screenSize = screenSize;
}

void CanvasRenderLayer::render () {
    uploadData();

    pipeline.bindSampler(samplerBinding, glyphAtlas.sampler());
    pipeline.bindUniform(uniformBinding, uniformBuffer);
    pipeline.render(indices.size());

    buffer.clear();
    indices.clear();
}

void CanvasRenderLayer::renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) {
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

void CanvasRenderLayer::renderConvexPoly (std::span<glm::vec2> points, glm::vec4 colour) {
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

void CanvasRenderLayer::renderConvexPolyAA (std::span<glm::vec2> points, glm::vec4 colour, float widthAA) {
    if (points.size() < 3 || colour.a <= 0.0f) {
        return;
    }

    // Render out in fan from start index
    const glm::vec3 opaque = glyphAtlas.opaque();

    std::uint16_t startIndex = 0;
    std::uint16_t lastIndex = 0;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto prev = points[(i - 1 + points.size()) % points.size()];
        auto curr = points[i];
        auto next = points[(i + 1) % points.size()];

        auto n1 = util::SafeNormalize({-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize({-(next.y - curr.y), next.x - curr.x});
        auto miter = -util::SafeNormalize(n1 + n2);

        std::uint16_t index = buffer.emplace(Vertex{
            .pos = curr - miter * widthAA,
            .uv = opaque,
            .colour = colour
        });
        buffer.emplace(Vertex{
            .pos = curr + miter * widthAA,
            .uv = opaque,
            .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
        });

        if (i == 0) {
            startIndex = index;
        } else {
            // Add AA geometry
            indices.emplace(lastIndex);
            indices.emplace(lastIndex + 1);
            indices.emplace(index);

            indices.emplace(lastIndex + 1);
            indices.emplace(index);
            indices.emplace(index + 1);
        }

        if (i >= 2) {
            // Add fan triangle
            indices.emplace(startIndex);
            indices.emplace(lastIndex);
            indices.emplace(index);
        }

        lastIndex = index;
    }

    // Close AA strip
    indices.emplace(lastIndex);
    indices.emplace(lastIndex + 1);
    indices.emplace(startIndex);

    indices.emplace(lastIndex + 1);
    indices.emplace(startIndex);
    indices.emplace(startIndex + 1);
}


void CanvasRenderLayer::renderPolyLine (std::span<glm::vec2> points, glm::vec4 colour, float width, bool closed) {
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width / 2.0f;
    const glm::vec3 opaque = glyphAtlas.opaque();
    std::uint16_t startIndex;
    std::uint16_t lastIndex;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto prev = i != 0 ? points[i - 1] : (closed ? points.back() : curr); // wrap around if closed, otherwise use curr
        auto next = i != points.size() - 1 ? points[i + 1] : (closed ? points.front() : curr); // wrap around if closed, otherwise use curr

        auto n1 = util::SafeNormalize(glm::vec2{-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize(glm::vec2{-(next.y - curr.y), next.x - curr.x});
        auto miter = glm::normalize(n1 + n2); // Average normals
        auto miterMult = 1.0f / glm::dot(miter, n1);
        auto miterLen = halfWidth * miterMult;

        std::uint16_t prevIndex;
        std::uint16_t nextIndex;
        if (miterMult <= MITER_LIMIT) {
            // Miter small enough, put in
            prevIndex = buffer.emplace(Vertex{
                   .pos = curr - miter * miterLen,
                   .uv = opaque,
                   .colour = colour
               });
            buffer.emplace(Vertex{
                .pos = curr + miter * miterLen,
                .uv = opaque,
                .colour = colour
            });

            nextIndex = prevIndex;
        } else {
            // Miter too large, use bevel joint
            prevIndex = buffer.emplace(Vertex{
                .pos = curr - n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr + n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });

            nextIndex = buffer.emplace(Vertex{
                .pos = curr - n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr + n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });

            // Add join geometry
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 1);
            indices.emplace(nextIndex);

            indices.emplace(prevIndex + 1);
            indices.emplace(nextIndex);
            indices.emplace(nextIndex + 1);
        }

        if (i == 0) {
            // No previous vertex
            startIndex = prevIndex;
        } else {
            // Render segment with previous vertex
            indices.emplace(lastIndex);
            indices.emplace(lastIndex + 1);
            indices.emplace(prevIndex);

            indices.emplace(lastIndex + 1);
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 1);
        }
        lastIndex = nextIndex;
    }

    if (closed) {
        // Complete outline
        indices.emplace(lastIndex);
        indices.emplace(lastIndex + 1);
        indices.emplace(startIndex);

        indices.emplace(lastIndex + 1);
        indices.emplace(startIndex);
        indices.emplace(startIndex + 1);
    }
}

void CanvasRenderLayer::renderPolyLineAA(std::span<glm::vec2> points, glm::vec4 colour, float width, bool closed, float widthAA) {
    // Doesnt do AA for non-closed lines properly
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width >= widthAA ? width / 2.0f - widthAA * 0.5f : 0.0f;
    const auto halfWidthAA = width >= widthAA ? width / 2.0f + widthAA * 0.5f : widthAA * 0.5f;
    const glm::vec3 opaque = glyphAtlas.opaque();

    std::uint16_t startIndex;
    std::uint16_t lastIndex;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto prev = i != 0 ? points[i - 1] : (closed ? points.back() : curr); // wrap around if closed, otherwise use curr
        auto next = i != points.size() - 1 ? points[i + 1] : (closed ? points.front() : curr); // wrap around if closed, otherwise use curr

        auto n1 = util::SafeNormalize(glm::vec2{-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize(glm::vec2{-(next.y - curr.y), next.x - curr.x});
        auto miter = glm::normalize(n1 + n2); // Average normals
        auto miterMult = 1.0f / glm::dot(miter, n1);
        auto miterLen = halfWidth * miterMult;
        auto miterLenAA = halfWidthAA * miterMult;

        std::uint16_t prevIndex;
        std::uint16_t nextIndex;
        if (miterMult <= MITER_LIMIT) {
            // Miter small enough, put in
            prevIndex = buffer.emplace(Vertex{
                   .pos = curr - miter * miterLen,
                   .uv = opaque,
                   .colour = colour
               });
            buffer.emplace(Vertex{
                   .pos = curr - miter * miterLenAA,
                   .uv = opaque,
                   .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
               });
            buffer.emplace(Vertex{
                .pos = curr + miter * miterLen,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                   .pos = curr + miter * miterLenAA,
                   .uv = opaque,
                   .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
               });

            nextIndex = prevIndex;
        } else {
            // Miter too large, use bevel joint
            prevIndex = buffer.emplace(Vertex{
                .pos = curr - n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr - n1 * halfWidthAA,
                .uv = opaque,
                .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
            });

            buffer.emplace(Vertex{
                .pos = curr + n1 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr + n1 * halfWidthAA,
                .uv = opaque,
                .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
            });

            nextIndex = buffer.emplace(Vertex{
                .pos = curr - n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr - n2 * halfWidthAA,
                .uv = opaque,
                .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
            });

            buffer.emplace(Vertex{
                .pos = curr + n2 * halfWidth,
                .uv = opaque,
                .colour = colour
            });
            buffer.emplace(Vertex{
                .pos = curr + n2 * halfWidthAA,
                .uv = opaque,
                .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}
            });

            // Add join geometry
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 2);
            indices.emplace(nextIndex);

            indices.emplace(prevIndex + 2);
            indices.emplace(nextIndex);
            indices.emplace(nextIndex + 2);

            // Add join AA
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 1);
            indices.emplace(nextIndex);

            indices.emplace(prevIndex + 1);
            indices.emplace(nextIndex);
            indices.emplace(nextIndex + 1);

            indices.emplace(prevIndex + 2);
            indices.emplace(prevIndex + 2 + 1);
            indices.emplace(nextIndex + 2);

            indices.emplace(prevIndex + 2 + 1);
            indices.emplace(nextIndex + 2);
            indices.emplace(nextIndex + 2 + 1);
        }

        if (i == 0) {
            // No previous vertex
            startIndex = prevIndex;
        } else {
            // Render segment with previous vertex
            indices.emplace(lastIndex);
            indices.emplace(lastIndex + 2);
            indices.emplace(prevIndex);

            indices.emplace(lastIndex + 2);
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 2);

            // Render AA
            indices.emplace(lastIndex);
            indices.emplace(lastIndex + 1);
            indices.emplace(prevIndex);

            indices.emplace(lastIndex + 1);
            indices.emplace(prevIndex);
            indices.emplace(prevIndex + 1);

            indices.emplace(lastIndex + 2);
            indices.emplace(lastIndex + 2 + 1);
            indices.emplace(prevIndex + 2);

            indices.emplace(lastIndex + 2 + 1);
            indices.emplace(prevIndex + 2);
            indices.emplace(prevIndex + 2 + 1);

        }
        lastIndex = nextIndex;
    }

    if (closed) {
        // Complete outline
        indices.emplace(lastIndex);
        indices.emplace(lastIndex + 2);
        indices.emplace(startIndex);

        indices.emplace(lastIndex + 2);
        indices.emplace(startIndex);
        indices.emplace(startIndex + 2);

        // Render AA
        indices.emplace(lastIndex);
        indices.emplace(lastIndex + 1);
        indices.emplace(startIndex);

        indices.emplace(lastIndex + 1);
        indices.emplace(startIndex);
        indices.emplace(startIndex + 1);

        indices.emplace(lastIndex + 2);
        indices.emplace(lastIndex + 2 + 1);
        indices.emplace(startIndex + 2);

        indices.emplace(lastIndex + 2 + 1);
        indices.emplace(startIndex + 2);
        indices.emplace(startIndex + 2 + 1);
    }
}

