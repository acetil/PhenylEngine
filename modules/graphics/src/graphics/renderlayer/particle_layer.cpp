#include "common/assets/assets.h"

#include "particle_layer.h"
#include "graphics/pipeline/pipeline.h"
#include "graphics/particles/particle_manager.h"

#define MAX_PARTICLES 512
#define VERTICES_PER_PARTICLE 6
#define MAX_VERTICES (MAX_PARTICLES * VERTICES_PER_PARTICLE)

namespace {
    struct Uniform {
        glm::mat4 camera;
    };
}

namespace phenyl::graphics {
    class ParticlePipeline : public Pipeline<ParticleManager2D> {
    private:
        Pipeline2 pipeline;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec4> colourBuffer;
        UniformBuffer<Uniform> uniformBuffer;

    public:
        ParticlePipeline () = default;

        void init (Renderer* renderer) override {
            BufferBinding posBinding;
            BufferBinding colourBinding;
            UniformBinding uniformBinding;
            auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/particle");
            pipeline = renderer->buildPipeline()
                    .withShader(shader)
                    .withBuffer<glm::vec2>(posBinding)
                    .withBuffer<glm::vec4>(colourBinding)
                    .withAttrib<glm::vec2>(0, posBinding)
                    .withAttrib<glm::vec4>(1, colourBinding)
                    .withUniform<Uniform>(shader->getUniformLocation("Camera"), uniformBinding)
                    .build();


            posBuffer = renderer->makeBuffer<glm::vec2>(MAX_VERTICES);
            colourBuffer = renderer->makeBuffer<glm::vec4>(MAX_VERTICES);
            uniformBuffer = renderer->makeUniformBuffer<Uniform>();

            pipeline.bindBuffer(posBinding, posBuffer);
            pipeline.bindBuffer(colourBinding, colourBuffer);
            pipeline.bindUniform(uniformBinding, uniformBuffer);
        }

        void applyCamera (const Camera& camera) {
            uniformBuffer->camera = camera.getCamMatrix();
        }

        void bufferData (phenyl::graphics::ParticleManager2D& manager) override {
            posBuffer.clear();
            colourBuffer.clear();

            manager.buffer(posBuffer, colourBuffer);

            posBuffer.upload();
            colourBuffer.upload();
        }

        void render() override {
            pipeline.render(posBuffer.size());
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
