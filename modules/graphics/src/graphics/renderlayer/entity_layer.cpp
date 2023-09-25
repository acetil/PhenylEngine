#include "entity_layer.h"
#include "common/components/2d/global_transform.h"
#include "graphics/components/2d/sprite.h"
#include "common/assets/assets.h"


#define BUFFER_SIZE (100 * 2 * 6)

using namespace phenyl;

static void bufferPosData (const phenyl::component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer);
static void bufferUvData (const phenyl::component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer);

namespace phenyl::graphics {
    class EntityPipeline : public Pipeline<phenyl::component::EntityComponentManager> {
    private:
        PipelineStage renderStage;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec2> uvBuffer;

    public:
        EntityPipeline () = default;

        void init (Renderer* renderer) override {
            renderStage = renderer->buildPipelineStage(PipelineStageBuilder(phenyl::common::Assets::Load<Shader>("phenyl/shaders/sprite"))
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

std::string graphics::EntityRenderLayer::getName () {
    return "entity_layer";
}

int graphics::EntityRenderLayer::getPriority () {
    return 2;
}

bool graphics::EntityRenderLayer::isActive () {
    return active;
}

void graphics::EntityRenderLayer::gatherData () {

}

void graphics::EntityRenderLayer::preRender (graphics::Renderer* renderer) {
    bool reloadNeeded = atlas.rebuild();

    componentManager->query<Sprite2D>().each([reloadNeeded] (auto entity, Sprite2D& sprite) {
        if (reloadNeeded || !sprite.updated) {
            sprite.update();
        }
    });

    entityPipeline->bufferData(*componentManager);
}

int graphics::EntityRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void graphics::EntityRenderLayer::applyUniform (int uniformId, void* data) {

}

void graphics::EntityRenderLayer::applyCamera (graphics::Camera camera) {
    entityPipeline->applyCamera(camera);
}

void graphics::EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    atlas.bind();
    entityPipeline->render();
}

graphics::EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::EntityComponentManager*componentManager) : componentManager{componentManager}, atlas{renderer} {
    this->entityPipeline = std::make_unique<EntityPipeline>();
    this->entityPipeline->init(renderer);
}

graphics::EntityRenderLayer::~EntityRenderLayer () = default;

static void bufferPosData (const component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer) {
    /*for (auto [model, transform] : manager.iterate<Model2D, common::GlobalTransform2D>()) {
        for (auto i : model.positionData) {
            buffer.pushData(transform.transform2D.apply(i));
        }
    }*/
    /*manager.query<common::GlobalTransform2D, Model2D>().each([&buffer] (auto info, const common::GlobalTransform2D& transform, const Model2D& model) {
        for (auto i : model.positionData) {
            buffer.pushData(transform.transform2D.apply(i));
        }
    });*/

    static glm::vec2 vertices[] =  {
            {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}
    };

    manager.query<common::GlobalTransform2D, graphics::Sprite2D>().each([&buffer] (auto info, const common::GlobalTransform2D& transform, const graphics::Sprite2D& sprite) {
        for (auto i : vertices) {
            buffer.pushData(transform.transform2D.apply(i));
        }
    });
}

static void bufferUvData (const component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer) {
    /*for (const auto& model : manager.iterate<Model2D>()) {
        buffer.pushData(model.uvData.begin(), model.uvData.end());
    }*/
    /*manager.query<Model2D>().each([&buffer] (auto info, const Model2D& model) {
        buffer.pushData(model.uvData.begin(), model.uvData.end());
    });*/

    manager.query<common::GlobalTransform2D, graphics::Sprite2D>().each([&buffer] (auto info, const common::GlobalTransform2D& transform, const graphics::Sprite2D& sprite) {
        auto topLeft = sprite.getTopLeft();
        auto bottomRight = sprite.getBottomRight();

        buffer.pushData({topLeft.x, topLeft.y});
        buffer.pushData({bottomRight.x, topLeft.y});
        buffer.pushData({topLeft.x, bottomRight.y});
        buffer.pushData({bottomRight.x, bottomRight.y});
        buffer.pushData({bottomRight.x, topLeft.y});
        buffer.pushData({topLeft.x, bottomRight.y});
    });
}