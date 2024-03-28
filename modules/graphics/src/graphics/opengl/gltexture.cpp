#include "gltexture.h"

using namespace phenyl::graphics;

static GLint GetGlFilter (TextureFilter filter) {
    switch (filter) {
        case TextureFilter::POINT:
            return GL_NEAREST;
        case TextureFilter::BILINEAR:
            return GL_LINEAR;
        case TextureFilter::TRILINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }

    PHENYL_ABORT("Invalid filter type: {}", (unsigned int)filter);
}

static GLint GetGlFormat (ImageFormat format) {
    switch (format) {
        case ImageFormat::R:
            return GL_RED;
        case ImageFormat::RGBA:
            return GL_RGBA;
    }

    PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
}

static GLenum GetGlFormatType (ImageFormat format) {
    switch (format) {
        case ImageFormat::R:
        case ImageFormat::RGBA:
            return GL_UNSIGNED_BYTE;
    }

    PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
}

GlTexture::GlTexture (const TextureProperties& properties) : properties{properties} {
    glGenTextures(1, &textureId);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGlFilter(properties.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GlTexture::GlTexture (GlTexture&& other) noexcept : textureId{other.textureId}, properties{other.properties}, texWidth{other.texWidth}, texHeight{other.texHeight} {
    other.textureId = 0;
    other.texWidth = 0;
    other.texHeight = 0;
}

GlTexture& GlTexture::operator= (GlTexture&& other) noexcept {
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }

    textureId = other.textureId;
    properties = other.properties;
    texWidth = other.texWidth;
    texHeight = other.texHeight;

    other.textureId = 0;
    other.texWidth = 0;
    other.texHeight = 0;

    return *this;
}

GlTexture::~GlTexture () {
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }
}

std::size_t GlTexture::hash () const noexcept {
    return textureId;
}

std::uint32_t GlTexture::width () const noexcept {
    return texWidth;
}

std::uint32_t GlTexture::height () const noexcept {
    return texHeight;
}

void GlTexture::upload (const Image& image) {
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GetGlFormat(properties.format), static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()), 0, GetGlFormat(image.format()),
                 GetGlFormatType(image.format()), image.data().data());

    if (properties.useMipmapping) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    texWidth = image.width();
    texHeight = image.height();
}
