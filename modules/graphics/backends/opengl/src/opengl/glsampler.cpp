#include "glsampler.h"

using namespace phenyl::graphics;
using namespace phenyl::opengl;

GlSampler::GlSampler (GLenum samplerType, const TextureProperties& properties) : m_properties{properties}, m_type{samplerType} {
    glGenTextures(1, &m_id);

    bind();
    glTexParameteri(type(), GL_TEXTURE_MIN_FILTER, filter());
    glTexParameteri(type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(type(), GL_TEXTURE_WRAP_S, GetGlWrapping(properties.wrapping));
    glTexParameteri(type(), GL_TEXTURE_WRAP_T, GetGlWrapping(properties.wrapping));
    auto borderColor = GetGlBorderColor(properties.borderColor);
    glTexParameterfv(type(), GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
}

GlSampler::GlSampler (GlSampler&& other) noexcept : m_id{other.m_id}, m_type{other.m_type}, m_properties{other.m_properties} {
    other.m_id = 0;
}

GlSampler& GlSampler::operator= (GlSampler&& other) noexcept {
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }

    m_id = other.m_id;
    m_properties = other.m_properties;

    other.m_id = 0;

    return *this;
}

GlSampler::~GlSampler () {
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
}

std::size_t GlSampler::hash () const noexcept {
    return m_id;
}

GLint GlSampler::internalFormat () const noexcept {
    return GetGlInternalFormat(properties().format);
}

GLint GlSampler::format () const noexcept {
    return GetGLFormat(properties().format);
}

GLenum GlSampler::formatType () const noexcept {
    return GetGlFormatType(properties().format);
}

GLint GlSampler::filter () const noexcept {
    switch (properties().filter) {
        case TextureFilter::POINT:
            return GL_NEAREST;
        case TextureFilter::BILINEAR:
            return GL_LINEAR;
        case TextureFilter::TRILINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }

    PHENYL_ABORT("Invalid filter type: {}", static_cast<std::uint32_t>(properties().filter));
}

void GlSampler::createEmpty2D (std::uint32_t width, std::uint32_t height) {
    bind();
    glTexImage2D(type(), 0, internalFormat(), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), 0, format(), formatType(), nullptr);

}

void GlSampler::upload (const Image& image) {
    bind();
    glTexImage2D(type(), 0, internalFormat(), static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()), 0, GetGLFormat(image.format()),
             GetGlFormatType(image.format()), image.data().data());

    if (properties().useMipmapping) {
        glGenerateMipmap(type());
    }
}

void GlSampler::createEmpty3D (std::uint32_t width, std::uint32_t height, std::uint32_t depth) {
    bind();
    glTexImage3D(type(), 0, internalFormat(), static_cast<GLint>(width), static_cast<GLint>(height), static_cast<GLint>(depth), 0, internalFormat(),
             formatType(), nullptr);
}

void GlSampler::uploadLayer (std::uint32_t layer, const Image& image) {
    bind();
    glTextureSubImage3D(id(), 0, 0, 0, static_cast<GLint>(layer), static_cast<GLint>(image.width()), static_cast<GLint>(image.height()), 1, GetGLFormat(image.format()),
                    GetGlFormatType(image.format()), image.data().data());

    if (properties().useMipmapping) {
        glGenerateMipmap(type());
    }
}

void GlSampler::bind () const {
    glBindTexture(m_type, m_id);
}
