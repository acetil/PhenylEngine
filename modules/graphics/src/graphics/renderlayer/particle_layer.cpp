#include "common/assets/assets.h"

#include "particle_layer.h"
#include "graphics/pipeline/pipeline.h"
#include "graphics/particles/particle_manager.h"

#define MAX_PARTICLES 3000
#define VERTICES_PER_PARTICLE 6
#define MAX_VERTICES (MAX_PARTICLES * VERTICES_PER_PARTICLE)

namespace phenyl::graphics {
    class ParticlePipeline : public Pipeline<ParticleManager2D> {
    private:
        PipelineStage renderStage;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec4> colourBuffer;

    public:
        ParticlePipeline () = default;

        void init (Renderer* renderer) override {
            renderStage = renderer->buildPipelineStage(PipelineStageBuilder{phenyl::common::Assets::Load<Shader>("phenyl/shaders/particle")}
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec4>(1));

            posBuffer = renderer->makeBuffer<glm::vec2>(MAX_VERTICES);
            colourBuffer = renderer->makeBuffer<glm::vec4>(MAX_VERTICES);

            renderStage.bindBuffer(0, posBuffer);
            renderStage.bindBuffer(1, colourBuffer);
        }

        void applyCamera (const Camera& camera) {
            renderStage.applyUniform(Camera::getUniformName(), camera.getCamMatrix());
        }

        void bufferData (phenyl::graphics::ParticleManager2D& manager) override {
            renderStage.clearBuffers();
            manager.buffer(posBuffer, colourBuffer);
            renderStage.bufferAllData();
        }

        void render() override {
            renderStage.render();
        }
    };
}

phenyl::graphics::ParticleRenderLayer::ParticleRenderLayer (phenyl::graphics::Renderer* renderer, ParticleManager2D* manager) : manager{manager}, pipeline{std::make_unique<ParticlePipeline>()} {
    PHENYL_DASSERT(manager);
    pipeline->init(renderer);
}

phenyl::graphics::ParticleRenderLayer::~ParticleRenderLayer () = default;

std::string phenyl::graphics::ParticleRenderLayer::getName () {
    return "particle_pipeline";
}

int phenyl::graphics::ParticleRenderLayer::getPriority () {
    return 3;
}

bool phenyl::graphics::ParticleRenderLayer::isActive () {
    return true;
}

void phenyl::graphics::ParticleRenderLayer::gatherData () {

}

void phenyl::graphics::ParticleRenderLayer::preRender (phenyl::graphics::Renderer* renderer) {
    pipeline->bufferData(*manager);
}

int phenyl::graphics::ParticleRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void phenyl::graphics::ParticleRenderLayer::applyUniform (int uniformId, void* data) {

}

void phenyl::graphics::ParticleRenderLayer::applyCamera (phenyl::graphics::Camera camera) {
    pipeline->applyCamera(camera);
}

void phenyl::graphics::ParticleRenderLayer::render (phenyl::graphics::Renderer* renderer, phenyl::graphics::FrameBuffer* frameBuf) {
    pipeline->render();
}
