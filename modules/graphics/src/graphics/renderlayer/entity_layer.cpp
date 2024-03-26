#include "entity_layer.h"
#include "common/components/2d/global_transform.h"
#include "graphics/components/2d/sprite.h"
#include "common/assets/assets.h"

#define MAX_ENTITIES 512
#define BUFFER_SIZE (MAX_ENTITIES * 2 * 6)

using namespace phenyl;

static void bufferPosData (const phenyl::component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer);
static void bufferUvData (const phenyl::component::EntityComponentManager& manager, graphics::Buffer<glm::vec2>& buffer);

namespace {
    struct Uniform {
        glm::mat4 camera;
    };
}

namespace phenyl::graphics {
    class EntityPipeline : public Pipeline<phenyl::component::EntityComponentManager, GraphicsTexture&> {
    private:
        Pipeline2 pipeline;

        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec2> uvBuffer;

        UniformBinding uniformBinding;
        UniformBuffer<Uniform> uniformBuffer;

        SamplerBinding samplerBinding;
        GraphicsTexture* texture;
    public:
        EntityPipeline () = default;

        void init (Renderer* renderer) override {
            BufferBinding posBinding;
            BufferBinding uvBinding;
            auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/sprite");
            pipeline = renderer->buildPipeline()
               .withShader(shader)
               .withBuffer<glm::vec2>(posBinding)
               .withBuffer<glm::vec2>(uvBinding)
               .withAttrib<glm::vec2>(0, posBinding)
               .withAttrib<glm::vec2>(1, uvBinding)
               .withUniform<Uniform>(shader->getUniformLocation("Camera"), uniformBinding)
               .withSampler2D(0, samplerBinding)
               .build();

            posBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            uvBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            uniformBuffer = renderer->makeUniformBuffer<Uniform>();

            pipeline.bindBuffer(posBinding, posBuffer);
            pipeline.bindBuffer(uvBinding, uvBuffer);
        }

        void applyCamera (graphics::Camera& camera) {
            uniformBuffer->camera = camera.getCamMatrix();
        }

        void bufferData (component::EntityComponentManager& manager, GraphicsTexture& texture) override {
            this->texture = &texture;

            posBuffer.clear();
            uvBuffer.clear();

            bufferPosData(manager, posBuffer);
            bufferUvData(manager, uvBuffer);

            posBuffer.upload();
            uvBuffer.upload();
        }

        void render () override {
            pipeline.bindUniform(uniformBinding, uniformBuffer);
            pipeline.bindSampler(samplerBinding, *texture);

            pipeline.render(posBuffer.size());
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

    entityPipeline->bufferData(*componentManager, atlas.getTexture());
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
    //atlas.bind();
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
            {-1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f}
    };

    manager.query<common::GlobalTransform2D, graphics::Sprite2D>().each([&buffer] (auto info, const common::GlobalTransform2D& transform, const graphics::Sprite2D& sprite) {
        for (auto i : vertices) {
            buffer.emplace(transform.transform2D.apply(i));
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

        buffer.emplace(topLeft.x, topLeft.y);
        buffer.emplace(bottomRight.x, topLeft.y);
        buffer.emplace(topLeft.x, bottomRight.y);
        buffer.emplace(bottomRight.x, bottomRight.y);
        buffer.emplace(bottomRight.x, topLeft.y);
        buffer.emplace(topLeft.x, bottomRight.y);
    });
}