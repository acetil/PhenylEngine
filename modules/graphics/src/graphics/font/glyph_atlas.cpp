#include "graphics/font/glyph_atlas.h"

using namespace phenyl::graphics;

ColumnAtlas::ColumnAtlas (std::uint32_t index, std::uint32_t size, std::uint32_t padding) :
    atlasImage{size, size, ImageFormat::R},
    index{index},
    padding{padding} {}

std::optional<glm::uvec2> ColumnAtlas::place (const Image& image) {
    Column* column = nullptr;

    for (auto& col : columns) {
        if (col.width >= image.width() && atlasImage.height() - col.currHeight >= image.height()) {
            column = &col;
            break;
        }
    }

    if (!column) {
        if (atlasImage.width() - currWidth < image.width()) {
            return std::nullopt;
        }

        column = &columns.emplace_back(Column{.offset = currWidth, .width = image.width(), .currHeight = 0});
        currWidth = std::min(atlasImage.width(), currWidth + image.width() + padding);
    }

    glm::uvec2 off{column->offset, column->currHeight};
    atlasImage.blit(image, off);

    column->currHeight = std::min(atlasImage.height(), column->currHeight + image.height() + padding);

    needsUpload = true;

    return off;
}

GlyphAtlas::GlyphAtlas (Renderer& renderer, std::uint32_t size, std::uint32_t padding) :
    m_arrayTexture{renderer.makeArrayTexture(
        TextureProperties{.format = ImageFormat::R, .filter = TextureFilter::POINT, .useMipmapping = true}, size,
        size)},
    m_size{size},
    m_padding{padding} {
    std::byte white{0xFF};
    auto [uvStart, _, atlasLayer] = place(Image::MakeNonOwning({&white, 1}, 1, 1, ImageFormat::R));
    m_whitePixel = glm::vec3{uvStart, atlasLayer};
}

ISampler& GlyphAtlas::sampler () const {
    return m_arrayTexture.sampler();
}

void GlyphAtlas::upload () {
    for (auto& atlas : m_atlases) {
        if (atlas.needsUpload) {
            m_arrayTexture.upload(atlas.index, atlas.atlasImage);
            atlas.needsUpload = false;
        }
    }
}

GlyphAtlas::Placement GlyphAtlas::place (const Image& image) {
    PHENYL_ASSERT_MSG(image.width() <= m_size && image.height() <= m_size,
        "Attempting to add glyph that is too large (size={}, glyphSize={}x{}", m_size, image.width(), image.height());

    for (auto& atlas : m_atlases) {
        auto off = atlas.place(image);
        if (off) {
            return Placement{.uvStart = glm::vec2{(float) off->x / (float) m_size, (float) off->y / (float) m_size},
              .uvEnd = glm::vec2{(float) (off->x + image.width()) / (float) m_size,
                (float) (off->y + image.height()) / (float) m_size},
              .atlasLayer = atlas.index};
        }
    }

    auto& atlas = m_atlases.emplace_back(m_arrayTexture.append(), m_size, m_padding);
    auto off = atlas.place(image);
    PHENYL_DASSERT(off);
    return Placement{.uvStart = glm::vec2{(float) off->x / (float) m_size, (float) off->y / (float) m_size},
      .uvEnd = glm::vec2{(float) (off->x + image.width()) / (float) m_size,
        (float) (off->y + image.height()) / (float) m_size},
      .atlasLayer = atlas.index};
}
