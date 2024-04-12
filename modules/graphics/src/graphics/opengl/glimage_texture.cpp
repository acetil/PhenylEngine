#include "glimage_texture.h"

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

GlImageTexture::GlImageTexture (const TextureProperties& properties) : texSampler{GL_TEXTURE_2D, properties} {
    texSampler.bind();
    glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, GetGlFilter(properties.filter));
    glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GlImageTexture::upload (const Image& image) {
    texSampler.bind();

    glTexImage2D(texSampler.type(), 0, GetGlFormat(texSampler.properties().format), static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()), 0, GetGlFormat(image.format()),
                 GetGlFormatType(image.format()), image.data().data());

    if (texSampler.properties().useMipmapping) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    texWidth = image.width();
    texHeight = image.height();
}

std::uint32_t GlImageTexture::width () const noexcept {
    return texWidth;
}

std::uint32_t GlImageTexture::height () const noexcept {
    return texHeight;
}

const ISampler& GlImageTexture::sampler () const noexcept {
    return texSampler;
}
