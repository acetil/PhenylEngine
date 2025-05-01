#include "core/assets/assets.h"

#include "particle_layer.h"

#include "graphics/camera_2d.h"

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
    auto shader = phenyl::core::Assets::Load<Shader>("phenyl/shaders/particle");
    m_pipeline = renderer.buildPipeline()
                       .withShader(shader)
                       .withBuffer<glm::vec2>(posBinding)
                       .withBuffer<glm::vec4>(colourBinding)
                       .withAttrib<glm::vec2>(0, posBinding)
                       .withAttrib<glm::vec4>(1, colourBinding)
                       .withUniform<Uniform>(shader->uniformLocation("Camera").value(), uniformBinding)
                       .build();


    m_posBuffer = renderer.makeBuffer<glm::vec2>(MAX_VERTICES, BufferStorageHint::DYNAMIC);
    m_colorBuffer = renderer.makeBuffer<glm::vec4>(MAX_VERTICES, BufferStorageHint::DYNAMIC);
    m_uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    m_pipeline.bindBuffer(posBinding, m_posBuffer);
    m_pipeline.bindBuffer(colourBinding, m_colorBuffer);
    m_pipeline.bindUniform(uniformBinding, m_uniformBuffer);
}

void ParticleRenderLayer::bufferData (const Camera2D& camera, const ParticleManager2D& manager) {
    m_posBuffer.clear();
    m_colorBuffer.clear();

    manager.buffer(m_posBuffer, m_colorBuffer);
    m_uniformBuffer->camera = camera.getCamMatrix();

    m_posBuffer.upload();
    m_colorBuffer.upload();
    m_uniformBuffer.upload();
}

void ParticleRenderLayer::render () {
    m_pipeline.render(m_posBuffer.size());
}
