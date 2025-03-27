#include "glimage_texture.h"

using namespace phenyl::graphics;

GlImageTexture::GlImageTexture (const TextureProperties& properties) : texSampler{GL_TEXTURE_2D, properties} {
    texSampler.bind();
    // glTexParameteri(texSampler.type(), GL_TEXTURE_MIN_FILTER, texSampler.filter());
    // glTexParameteri(texSampler.type(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GlImageTexture::upload (const Image& image) {
    // texSampler.bind();
    //
    // glTexImage2D(texSampler.type(), 0, texSampler.format(), static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()), 0, GetGlFormat(image.format()),
    //              GetGlFormatType(image.format()), image.data().data());
    //
    // if (texSampler.properties().useMipmapping) {
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    texSampler.upload(image);

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
