#include "glarray_texture.h"

using namespace phenyl::opengl;

#define STARTING_DEPTH 8
#define RESIZE_FACTOR (3 / 2)

GlArrayTexture::GlArrayTexture (const graphics::TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight) : m_sampler{GL_TEXTURE_2D_ARRAY, properties}, m_width{texWidth}, m_height{texHeight}, m_size{0}, m_capacity{STARTING_DEPTH} {
    // texSampler.bind();
    // // glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, texSampler.filter());
    // // glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexImage3D(texSampler.type(), 0, texSampler.format(), static_cast<GLint>(texWidth), static_cast<GLint>(texHeight), static_cast<GLint>(depthCapacity), 0, texSampler.format(),
    //              texSampler.formatType(), nullptr);

    m_sampler.createEmpty3D(texWidth, texHeight, m_capacity);
}

std::uint32_t GlArrayTexture::width () const noexcept {
    return m_width;
}

std::uint32_t GlArrayTexture::height () const noexcept {
    return m_height;
}

std::uint32_t GlArrayTexture::size () const noexcept {
    return m_size;
}

void GlArrayTexture::reserve (std::uint32_t capacity) {
    if (capacity <= m_capacity) {
        return;
    }

    GlSampler newSampler{GL_TEXTURE_2D_ARRAY, m_sampler.properties()};
    // newSampler.bind();
    // glTexParameteri(newSampler.type(), GL_TEXTURE_MIN_FILTER, newSampler.filter());
    // glTexParameteri(newSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexImage3D(newSampler.type(), 0, newSampler.format(), static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(capacity), 0, newSampler.format(),
    //              newSampler.formatType(), nullptr);
    newSampler.createEmpty3D(width(), height(), capacity);

    glCopyImageSubData(m_sampler.id(), newSampler.type(), 0, 0, 0, 0, newSampler.id(), newSampler.type(), 0, 0, 0, 0, static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(m_capacity));

    m_sampler = std::move(newSampler);
    m_capacity = capacity;

    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(texSampler.type());
    // }
}

std::uint32_t GlArrayTexture::append () {
    if (m_size == m_capacity) {
        reserve(m_capacity * RESIZE_FACTOR);
    }

    return m_size++;
}

void GlArrayTexture::upload (std::uint32_t index, const graphics::Image& image) {
    PHENYL_ASSERT_MSG(index < m_size, "Invalid array index={} (array size={})", index, m_size);
    PHENYL_ASSERT_MSG(image.width() == width() && image.height() == height(), "Invalid image dimensions: array is {}x{} but image is {}x{}", width(), height(), image.width(), image.height());

    // glTextureSubImage3D(texSampler.id(), 0, 0, 0, static_cast<GLint>(index), static_cast<GLint>(width()), static_cast<GLint>(height()), 1, GetGlFormat(image.format()),
    //                     GetGlFormatType(image.format()), image.data().data());
    //
    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(texSampler.type());
    // }
    m_sampler.uploadLayer(index, image);
}

phenyl::graphics::ISampler& GlArrayTexture::sampler () noexcept {
    return m_sampler;
}
