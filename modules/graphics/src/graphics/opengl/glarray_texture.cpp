#include "glarray_texture.h"

using namespace phenyl::graphics;

#define STARTING_DEPTH 8
#define RESIZE_FACTOR (3 / 2)

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

GlArrayTexture::GlArrayTexture (const TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight) : texSampler{GL_TEXTURE_2D_ARRAY, properties}, texWidth{texWidth}, texHeight{texHeight}, depth{0}, depthCapacity{STARTING_DEPTH} {
    texSampler.bind();
    glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, GetGlFilter(properties.filter));
    glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(texSampler.type(), 0, GetGlFormat(texSampler.properties().format), static_cast<GLint>(texWidth), static_cast<GLint>(texHeight), static_cast<GLint>(depthCapacity), 0, GetGlFormat(texSampler.properties().format),
                 GetGlFormatType(texSampler.properties().format), nullptr);
}

std::uint32_t GlArrayTexture::width () const noexcept {
    return texWidth;
}

std::uint32_t GlArrayTexture::height () const noexcept {
    return texHeight;
}

std::uint32_t GlArrayTexture::size () const noexcept {
    return depth;
}

void GlArrayTexture::reserve (std::uint32_t capacity) {
    if (capacity <= depthCapacity) {
        return;
    }

    GlSampler newSampler{GL_TEXTURE_2D_ARRAY, texSampler.properties()};
    newSampler.bind();
    glTexParameteri(newSampler.type(), GL_TEXTURE_MIN_FILTER, GetGlFilter(newSampler.properties().filter));
    glTexParameteri(newSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(newSampler.type(), 0, GetGlFormat(newSampler.properties().format), static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(capacity), 0, GetGlFormat(newSampler.properties().format),
                 GetGlFormatType(newSampler.properties().format), nullptr);
    glCopyImageSubData(texSampler.id(), newSampler.type(), 0, 0, 0, 0, newSampler.id(), newSampler.type(), 0, 0, 0, 0, static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(depthCapacity));

    texSampler = std::move(newSampler);
    depthCapacity = capacity;

    if (texSampler.properties().useMipmapping) {
        glGenerateMipmap(texSampler.type());
    }
}

std::uint32_t GlArrayTexture::append () {
    if (depth == depthCapacity) {
        reserve(depthCapacity * RESIZE_FACTOR);
    }

    return depth++;
}

void GlArrayTexture::upload (std::uint32_t index, const Image& image) {
    PHENYL_ASSERT_MSG(index < depth, "Invalid array index={} (array size={})", index, depth);
    PHENYL_ASSERT_MSG(image.width() == width() && image.height() == height(), "Invalid image dimensions: array is {}x{} but image is {}x{}", width(), height(), image.width(), image.height());

    glTextureSubImage3D(texSampler.id(), 0, 0, 0, static_cast<GLint>(index), static_cast<GLint>(width()), static_cast<GLint>(height()), 1, GetGlFormat(image.format()),
                        GetGlFormatType(image.format()), image.data().data());

    if (texSampler.properties().useMipmapping) {
        glGenerateMipmap(texSampler.type());
    }
}

const ISampler& GlArrayTexture::sampler () const noexcept {
    return texSampler;
}
