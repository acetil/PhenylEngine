#include "graphics/renderlayer/canvas_layer.h"

#include "core/assets/assets.h"
#include "graphics/font/glyph_atlas.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 4)
#define MITER_LIMIT 2.5f

CanvasRenderLayer::CanvasRenderLayer (GlyphAtlas& glyphAtlas) : AbstractRenderLayer{4}, m_glyphAtlas{glyphAtlas} {}

std::string_view CanvasRenderLayer::getName () const {
    return "CanvasRenderLayer";
}

void CanvasRenderLayer::init (Renderer& renderer) {
    BufferBinding textBinding;
    auto textShader = phenyl::core::Assets::Load<Shader>("phenyl/shaders/canvas");
    m_pipeline = renderer.buildPipeline()
                     .withShader(textShader)
                     .withBuffer<Vertex>(textBinding)
                     .withAttrib<glm::vec2>(0, textBinding, offsetof(Vertex, pos))
                     .withAttrib<glm::vec3>(1, textBinding, offsetof(Vertex, uv))
                     .withAttrib<glm::vec4>(2, textBinding, offsetof(Vertex, colour))
                     .withSampler2D(textShader->samplerLocation("textureSampler").value(), m_samplerBinding)
                     .withUniform<Uniform>(textShader->uniformLocation("Uniform").value(), m_uniformBinding)
                     .withBlending(BlendMode::ALPHA_BLEND)
                     .build();

    m_buffer = renderer.makeBuffer<Vertex>(BUFFER_SIZE, BufferStorageHint::DYNAMIC);
    m_indices = renderer.makeBuffer<std::uint16_t>(BUFFER_SIZE, BufferStorageHint::DYNAMIC, true);
    m_uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    m_pipeline.bindBuffer(textBinding, m_buffer);
    m_pipeline.bindIndexBuffer(m_indices);
    m_pipeline.bindSampler(m_samplerBinding, m_glyphAtlas.sampler());
}

void CanvasRenderLayer::uploadData () {
    m_buffer.upload();
    m_indices.upload();
    m_glyphAtlas.upload();
}

void CanvasRenderLayer::setScreenSize (glm::vec2 screenSize) {
    m_uniformBuffer->screenSize = screenSize;
    m_uniformBuffer.upload();
}

void CanvasRenderLayer::render () {
    uploadData();

    m_pipeline.bindSampler(m_samplerBinding, m_glyphAtlas.sampler());
    m_pipeline.bindUniform(m_uniformBinding, m_uniformBuffer);
    m_pipeline.render(m_indices.size());

    m_buffer.clear();
    m_indices.clear();
}

void CanvasRenderLayer::renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) {
    auto topLeft = pos;
    auto bottomRight = pos + glyph.size;

    auto startIndex = m_buffer.emplace(
        Vertex{.pos = topLeft, .uv = glm::vec3{glyph.uvStart, glyph.atlasLayer}, .colour = glm::vec4{colour, 1.0f}});
    m_buffer.emplace(Vertex{.pos = glm::vec2{bottomRight.x, topLeft.y},
      .uv = glm::vec3{glyph.uvEnd.x, glyph.uvStart.y, glyph.atlasLayer},
      .colour = glm::vec4{colour, 1.0f}});
    m_buffer.emplace(Vertex{.pos = glm::vec2{topLeft.x, bottomRight.y},
      .uv = glm::vec3{glyph.uvStart.x, glyph.uvEnd.y, glyph.atlasLayer},
      .colour = glm::vec4{colour, 1.0f}});
    m_buffer.emplace(
        Vertex{.pos = bottomRight, .uv = glm::vec3{glyph.uvEnd, glyph.atlasLayer}, .colour = glm::vec4{colour, 1.0f}});

    m_indices.emplace(startIndex + 0);
    m_indices.emplace(startIndex + 1);
    m_indices.emplace(startIndex + 2);

    m_indices.emplace(startIndex + 1);
    m_indices.emplace(startIndex + 2);
    m_indices.emplace(startIndex + 3);
}

void CanvasRenderLayer::renderConvexPoly (std::span<const glm::vec2> points, glm::vec4 colour) {
    if (points.size() < 3 || colour.a <= 0.0f) {
        return;
    }

    // Render out in fan from start index
    const glm::vec3 opaque = m_glyphAtlas.opaque();
    std::uint16_t startIndex = m_buffer.emplace(Vertex{.pos = points[0], .uv = opaque, .colour = colour});
    std::uint16_t lastIndex = m_buffer.emplace(Vertex{.pos = points[1], .uv = opaque, .colour = colour});

    for (std::size_t i = 2; i < points.size(); i++) {
        std::uint16_t index = m_buffer.emplace(Vertex{.pos = points[i], .uv = opaque, .colour = colour});

        // Add next fan triangle
        m_indices.emplace(startIndex);
        m_indices.emplace(lastIndex);
        m_indices.emplace(index);

        lastIndex = index;
    }
}

void CanvasRenderLayer::renderConvexPolyAA (std::span<const glm::vec2> points, glm::vec4 colour, float widthAA) {
    if (points.size() < 3 || colour.a <= 0.0f) {
        return;
    }

    // Render out in fan from start index
    const glm::vec3 opaque = m_glyphAtlas.opaque();

    std::uint16_t startIndex = 0;
    std::uint16_t lastIndex = 0;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto prev = points[(i - 1 + points.size()) % points.size()];
        auto curr = points[i];
        auto next = points[(i + 1) % points.size()];

        auto n1 = util::SafeNormalize({-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize({-(next.y - curr.y), next.x - curr.x});
        auto miter = -util::SafeNormalize(n1 + n2);

        std::uint16_t index = m_buffer.emplace(Vertex{.pos = curr - miter * widthAA, .uv = opaque, .colour = colour});
        m_buffer.emplace(Vertex{.pos = curr + miter * widthAA,
          .uv = opaque,
          .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

        if (i == 0) {
            startIndex = index;
        } else {
            // Add AA geometry
            m_indices.emplace(lastIndex);
            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(index);

            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(index);
            m_indices.emplace(index + 1);
        }

        if (i >= 2) {
            // Add fan triangle
            m_indices.emplace(startIndex);
            m_indices.emplace(lastIndex);
            m_indices.emplace(index);
        }

        lastIndex = index;
    }

    // Close AA strip
    m_indices.emplace(lastIndex);
    m_indices.emplace(lastIndex + 1);
    m_indices.emplace(startIndex);

    m_indices.emplace(lastIndex + 1);
    m_indices.emplace(startIndex);
    m_indices.emplace(startIndex + 1);
}

void CanvasRenderLayer::renderPolyLine (std::span<const glm::vec2> points, glm::vec4 colour, float width, bool closed) {
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width / 2.0f;
    const glm::vec3 opaque = m_glyphAtlas.opaque();
    std::uint16_t startIndex;
    std::uint16_t lastIndex;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto prev =
            i != 0 ? points[i - 1] : (closed ? points.back() : curr); // wrap around if closed, otherwise use curr
        auto next = i != points.size() - 1 ?
            points[i + 1] :
            (closed ? points.front() : curr); // wrap around if closed, otherwise use curr

        auto n1 = util::SafeNormalize(glm::vec2{-(curr.y - prev.y), curr.x - prev.x});
        auto n2 = util::SafeNormalize(glm::vec2{-(next.y - curr.y), next.x - curr.x});
        auto miter = glm::normalize(n1 + n2); // Average normals
        auto miterMult = 1.0f / glm::dot(miter, n1);
        auto miterLen = halfWidth * miterMult;

        std::uint16_t prevIndex;
        std::uint16_t nextIndex;
        if (miterMult <= MITER_LIMIT) {
            // Miter small enough, put in
            prevIndex = m_buffer.emplace(Vertex{.pos = curr - miter * miterLen, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + miter * miterLen, .uv = opaque, .colour = colour});

            nextIndex = prevIndex;
        } else {
            // Miter too large, use bevel joint
            prevIndex = m_buffer.emplace(Vertex{.pos = curr - n1 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + n1 * halfWidth, .uv = opaque, .colour = colour});

            nextIndex = m_buffer.emplace(Vertex{.pos = curr - n2 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + n2 * halfWidth, .uv = opaque, .colour = colour});

            // Add join geometry
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 1);
            m_indices.emplace(nextIndex);

            m_indices.emplace(prevIndex + 1);
            m_indices.emplace(nextIndex);
            m_indices.emplace(nextIndex + 1);
        }

        if (i == 0) {
            // No previous vertex
            startIndex = prevIndex;
        } else {
            // Render segment with previous vertex
            m_indices.emplace(lastIndex);
            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(prevIndex);

            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 1);
        }
        lastIndex = nextIndex;
    }

    if (closed) {
        // Complete outline
        m_indices.emplace(lastIndex);
        m_indices.emplace(lastIndex + 1);
        m_indices.emplace(startIndex);

        m_indices.emplace(lastIndex + 1);
        m_indices.emplace(startIndex);
        m_indices.emplace(startIndex + 1);
    }
}

void CanvasRenderLayer::renderPolyLineAA (std::span<const glm::vec2> points, glm::vec4 colour, float width, bool closed,
    float widthAA) {
    // Doesnt do AA for non-closed lines properly
    if (points.size() <= 2 || colour.a <= 0.0f || width <= 0.0f) {
        return;
    }

    const auto halfWidth = width >= widthAA ? width / 2.0f - widthAA * 0.5f : 0.0f;
    const auto halfWidthAA = width >= widthAA ? width / 2.0f + widthAA * 0.5f : widthAA * 0.5f;
    const glm::vec3 opaque = m_glyphAtlas.opaque();

    std::uint16_t startIndex;
    std::uint16_t lastIndex;
    for (std::size_t i = 0; i < points.size(); i++) {
        auto curr = points[i];
        auto prev =
            i != 0 ? points[i - 1] : (closed ? points.back() : curr); // wrap around if closed, otherwise use curr
        auto next = i != points.size() - 1 ?
            points[i + 1] :
            (closed ? points.front() : curr); // wrap around if closed, otherwise use curr

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
            prevIndex = m_buffer.emplace(Vertex{.pos = curr - miter * miterLen, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr - miter * miterLenAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});
            m_buffer.emplace(Vertex{.pos = curr + miter * miterLen, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + miter * miterLenAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

            nextIndex = prevIndex;
        } else {
            // Miter too large, use bevel joint
            prevIndex = m_buffer.emplace(Vertex{.pos = curr - n1 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr - n1 * halfWidthAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

            m_buffer.emplace(Vertex{.pos = curr + n1 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + n1 * halfWidthAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

            nextIndex = m_buffer.emplace(Vertex{.pos = curr - n2 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr - n2 * halfWidthAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

            m_buffer.emplace(Vertex{.pos = curr + n2 * halfWidth, .uv = opaque, .colour = colour});
            m_buffer.emplace(Vertex{.pos = curr + n2 * halfWidthAA,
              .uv = opaque,
              .colour = glm::vec4{colour.r, colour.g, colour.b, 0.0f}});

            // Add join geometry
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 2);
            m_indices.emplace(nextIndex);

            m_indices.emplace(prevIndex + 2);
            m_indices.emplace(nextIndex);
            m_indices.emplace(nextIndex + 2);

            // Add join AA
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 1);
            m_indices.emplace(nextIndex);

            m_indices.emplace(prevIndex + 1);
            m_indices.emplace(nextIndex);
            m_indices.emplace(nextIndex + 1);

            m_indices.emplace(prevIndex + 2);
            m_indices.emplace(prevIndex + 2 + 1);
            m_indices.emplace(nextIndex + 2);

            m_indices.emplace(prevIndex + 2 + 1);
            m_indices.emplace(nextIndex + 2);
            m_indices.emplace(nextIndex + 2 + 1);
        }

        if (i == 0) {
            // No previous vertex
            startIndex = prevIndex;
        } else {
            // Render segment with previous vertex
            m_indices.emplace(lastIndex);
            m_indices.emplace(lastIndex + 2);
            m_indices.emplace(prevIndex);

            m_indices.emplace(lastIndex + 2);
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 2);

            // Render AA
            m_indices.emplace(lastIndex);
            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(prevIndex);

            m_indices.emplace(lastIndex + 1);
            m_indices.emplace(prevIndex);
            m_indices.emplace(prevIndex + 1);

            m_indices.emplace(lastIndex + 2);
            m_indices.emplace(lastIndex + 2 + 1);
            m_indices.emplace(prevIndex + 2);

            m_indices.emplace(lastIndex + 2 + 1);
            m_indices.emplace(prevIndex + 2);
            m_indices.emplace(prevIndex + 2 + 1);
        }
        lastIndex = nextIndex;
    }

    if (closed) {
        // Complete outline
        m_indices.emplace(lastIndex);
        m_indices.emplace(lastIndex + 2);
        m_indices.emplace(startIndex);

        m_indices.emplace(lastIndex + 2);
        m_indices.emplace(startIndex);
        m_indices.emplace(startIndex + 2);

        // Render AA
        m_indices.emplace(lastIndex);
        m_indices.emplace(lastIndex + 1);
        m_indices.emplace(startIndex);

        m_indices.emplace(lastIndex + 1);
        m_indices.emplace(startIndex);
        m_indices.emplace(startIndex + 1);

        m_indices.emplace(lastIndex + 2);
        m_indices.emplace(lastIndex + 2 + 1);
        m_indices.emplace(startIndex + 2);

        m_indices.emplace(lastIndex + 2 + 1);
        m_indices.emplace(startIndex + 2);
        m_indices.emplace(startIndex + 2 + 1);
    }
}
