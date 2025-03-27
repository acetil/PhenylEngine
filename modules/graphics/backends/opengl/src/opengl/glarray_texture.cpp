#include "glarray_texture.h"

using namespace phenyl::graphics;

#define STARTING_DEPTH 8
#define RESIZE_FACTOR (3 / 2)

GlArrayTexture::GlArrayTexture (const TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight) : texSampler{GL_TEXTURE_2D_ARRAY, properties}, texWidth{texWidth}, texHeight{texHeight}, depth{0}, depthCapacity{STARTING_DEPTH} {
    // texSampler.bind();
    // // glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, texSampler.filter());
    // // glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexImage3D(texSampler.type(), 0, texSampler.format(), static_cast<GLint>(texWidth), static_cast<GLint>(texHeight), static_cast<GLint>(depthCapacity), 0, texSampler.format(),
    //              texSampler.formatType(), nullptr);

    texSampler.createEmpty3D(texWidth, texHeight, depthCapacity);
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
    // newSampler.bind();
    // glTexParameteri(newSampler.type(), GL_TEXTURE_MIN_FILTER, newSampler.filter());
    // glTexParameteri(newSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexImage3D(newSampler.type(), 0, newSampler.format(), static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(capacity), 0, newSampler.format(),
    //              newSampler.formatType(), nullptr);
    newSampler.createEmpty3D(width(), height(), capacity);

    glCopyImageSubData(texSampler.id(), newSampler.type(), 0, 0, 0, 0, newSampler.id(), newSampler.type(), 0, 0, 0, 0, static_cast<GLint>(width()), static_cast<GLint>(height()), static_cast<GLint>(depthCapacity));

    texSampler = std::move(newSampler);
    depthCapacity = capacity;

    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(texSampler.type());
    // }
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

    // glTextureSubImage3D(texSampler.id(), 0, 0, 0, static_cast<GLint>(index), static_cast<GLint>(width()), static_cast<GLint>(height()), 1, GetGlFormat(image.format()),
    //                     GetGlFormatType(image.format()), image.data().data());
    //
    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(texSampler.type());
    // }
    texSampler.uploadLayer(index, image);
}

const ISampler& GlArrayTexture::sampler () const noexcept {
    return texSampler;
}
