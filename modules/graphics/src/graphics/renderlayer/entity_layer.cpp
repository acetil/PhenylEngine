#include "entity_layer.h"
#include "common/components/2d/global_transform.h"


#define BUFFER_SIZE (100 * 2 * 6)

using namespace graphics;

static void bufferPosData (const component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer);
static void bufferUvData (const component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer);

namespace graphics {
    class EntityPipeline : public Pipeline<component::EntityComponentManager> {
    private:
        PipelineStage renderStage;
        ShaderProgramNew shader;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec2> uvBuffer;

    public:
        EntityPipeline () = default;

        explicit EntityPipeline (const ShaderProgramNew& _shader) : shader{_shader} {};

        void init (Renderer* renderer) override {
            renderStage = renderer->buildPipelineStage(PipelineStageBuilder(shader)
                                                               .addVertexAttrib<glm::vec2>(0)
                                                               .addVertexAttrib<glm::vec2>(1));
                                                               //.addVertexAttrib<glm::vec2>(2)
                                                               //.addVertexAttrib<glm::mat2>(3)));

            posBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            uvBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);

            renderStage.bindBuffer(0, posBuffer);
            renderStage.bindBuffer(1, uvBuffer);
        }

        void applyCamera (graphics::Camera& camera) {
            renderStage.applyUniform(Camera::getUniformName(), camera.getCamMatrix());
        }

        void bufferData (component::EntityComponentManager& manager) override {
            renderStage.clearBuffers();

            bufferPosData(manager, posBuffer);
            bufferUvData(manager, uvBuffer);

            renderStage.bufferAllData();
        }

        void render () override {
            renderStage.render();
        }
    };
}

std::string EntityRenderLayer::getName () {
    return "entity_layer";
}

int EntityRenderLayer::getPriority () {
    return 2;
}

bool graphics::EntityRenderLayer::isActive () {
    return active;
}

void EntityRenderLayer::gatherData () {

}

void EntityRenderLayer::preRender (graphics::Renderer* renderer) {
    entityPipeline->bufferData(*componentManager);
}

int EntityRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void EntityRenderLayer::applyUniform (int uniformId, void* data) {

}

void EntityRenderLayer::applyCamera (graphics::Camera camera) {
    entityPipeline->applyCamera(camera);
}

void EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    entityPipeline->render();
}

EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::EntityComponentManager*componentManager) : shaderProgram{renderer->getProgramNew("default").orThrow()}, componentManager{componentManager} {
    this->entityPipeline = std::make_unique<EntityPipeline>(shaderProgram);
    this->entityPipeline->init(renderer);
}

EntityRenderLayer::~EntityRenderLayer () = default;

static void bufferPosData (const component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer) {
    /*for (auto [model, transform] : manager.iterate<Model2D, common::GlobalTransform2D>()) {
        for (auto i : model.positionData) {
            buffer.pushData(transform.transform2D.apply(i));
        }
    }*/
    manager.query<common::GlobalTransform2D, Model2D>().each([&buffer] (auto info, const common::GlobalTransform2D& transform, const Model2D& model) {
        for (auto i : model.positionData) {
            buffer.pushData(transform.transform2D.apply(i));
        }
    });
}

static void bufferUvData (const component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer) {
    /*for (const auto& model : manager.iterate<Model2D>()) {
        buffer.pushData(model.uvData.begin(), model.uvData.end());
    }*/
    manager.query<Model2D>().each([&buffer] (auto info, const Model2D& model) {
        buffer.pushData(model.uvData.begin(), model.uvData.end());
    });
}