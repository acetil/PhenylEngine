#include "common/assets/assets.h"

#include "particle_layer.h"
#include "graphics/particles/particle_manager.h"

#define MAX_PARTICLES 512
#define VERTICES_PER_PARTICLE 6
#define MAX_VERTICES (MAX_PARTICLES * VERTICES_PER_PARTICLE)

using namespace phenyl::graphics;

ParticleRenderLayer::ParticleRenderLayer () : AbstractRenderLayer{2} {}

std::string_view ParticleRenderLayer::getName () const {
    return "ParticleRenderLayer";
}

void ParticleRenderLayer::init (Renderer& renderer) {
    BufferBinding posBinding;
    BufferBinding colourBinding;
    UniformBinding uniformBinding;
    auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/particle");
    pipeline = renderer.buildPipeline()
                       .withShader(shader)
                       .withBuffer<glm::vec2>(posBinding)
                       .withBuffer<glm::vec4>(colourBinding)
                       .withAttrib<glm::vec2>(0, posBinding)
                       .withAttrib<glm::vec4>(1, colourBinding)
                       .withUniform<Uniform>(shader->getUniformLocation("Camera"), uniformBinding)
                       .build();


    posBuffer = renderer.makeBuffer<glm::vec2>(MAX_VERTICES);
    colourBuffer = renderer.makeBuffer<glm::vec4>(MAX_VERTICES);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    pipeline.bindBuffer(posBinding, posBuffer);
    pipeline.bindBuffer(colourBinding, colourBuffer);
    pipeline.bindUniform(uniformBinding, uniformBuffer);
}

void ParticleRenderLayer::bufferData (const ParticleManager2D& manager) {
    posBuffer.clear();
    colourBuffer.clear();

    manager.buffer(posBuffer, colourBuffer);

    posBuffer.upload();
    colourBuffer.upload();
}

void ParticleRenderLayer::render () {
    pipeline.render(posBuffer.size());
}
