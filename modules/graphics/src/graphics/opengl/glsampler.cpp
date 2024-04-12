#include "glsampler.h"

using namespace phenyl::graphics;

GlSampler::GlSampler (GLenum samplerType, const TextureProperties& properties) : samplerProperties{properties}, samplerType{samplerType} {
    glGenTextures(1, &textureId);
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

void GlSampler::bind () const {
    glBindTexture(samplerType, textureId);
}