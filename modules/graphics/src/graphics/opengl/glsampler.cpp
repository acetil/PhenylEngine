#include "glsampler.h"

using namespace phenyl::graphics;

GlSampler::GlSampler (GLenum samplerType, const TextureProperties& properties) : samplerProperties{properties}, samplerType{samplerType} {
    glGenTextures(1, &textureId);

    bind();
    glTexParameteri(type(), GL_TEXTURE_MIN_FILTER, filter());
    glTexParameteri(type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(type(), GL_TEXTURE_WRAP_S, GetGlWrapping(properties.wrapping));
    glTexParameteri(type(), GL_TEXTURE_WRAP_T, GetGlWrapping(properties.wrapping));
    glTexParameterfv(type(), GL_TEXTURE_BORDER_COLOR, &properties.borderColor[0]);
}

GlSampler::GlSampler (GlSampler&& other) noexcept : textureId{other.textureId}, samplerType{other.samplerType}, samplerProperties{other.samplerProperties} {
    other.textureId = 0;
}

GlSampler& GlSampler::operator= (GlSampler&& other) noexcept {
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }

    textureId = other.textureId;
    samplerProperties = other.samplerProperties;

    other.textureId = 0;

    return *this;
}

GlSampler::~GlSampler () {
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }
}

std::size_t GlSampler::hash () const noexcept {
    return textureId;
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
    glBindTexture(samplerType, textureId);
}
