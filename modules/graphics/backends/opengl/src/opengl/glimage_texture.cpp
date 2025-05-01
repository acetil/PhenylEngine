#include "glimage_texture.h"

using namespace phenyl::opengl;

GlImageTexture::GlImageTexture (const graphics::TextureProperties& properties) : m_sampler{GL_TEXTURE_2D, properties} {
    m_sampler.bind();
    // glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, texSampler.filter());
    // glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GlImageTexture::upload (const graphics::Image& image) {
    // texSampler.bind();
    //
    // glTexImage2D(texSampler.type(), 0, texSampler.format(), static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()), 0, GetGlFormat(image.format()),
    //              GetGlFormatType(image.format()), image.data().data());
    //
    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    m_sampler.upload(image);

    m_width = image.width();
    m_height = image.height();
}

std::uint32_t GlImageTexture::width () const noexcept {
    return m_width;
}

std::uint32_t GlImageTexture::height () const noexcept {
    return m_height;
}

phenyl::graphics::ISampler& GlImageTexture::sampler () noexcept {
    return m_sampler;
}
