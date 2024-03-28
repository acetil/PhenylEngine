#include "entity_layer.h"
#include "common/components/2d/global_transform.h"
#include "graphics/components/2d/sprite.h"
#include "common/assets/assets.h"

#define MAX_ENTITIES 512
#define BUFFER_SIZE (MAX_ENTITIES * 2 * 6)

using namespace phenyl::graphics;

static void bufferPosData (const phenyl::component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer);
static void bufferUvData (const phenyl::component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer);

EntityRenderLayer::EntityRenderLayer () : AbstractRenderLayer{0} {}

std::string_view EntityRenderLayer::getName () const {
    return "EntityRenderLayer";
}

void EntityRenderLayer::init (Renderer& renderer) {
    atlas = std::make_unique<SpriteAtlas>(&renderer);

    BufferBinding posBinding;
    BufferBinding uvBinding;
    auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/sprite");
    pipeline = renderer.buildPipeline()
                       .withShader(shader)
                       .withBuffer<glm::vec2>(posBinding)
                       .withBuffer<glm::vec2>(uvBinding)
                       .withAttrib<glm::vec2>(0, posBinding)
                       .withAttrib<glm::vec2>(1, uvBinding)
                       .withUniform<Uniform>(shader->getUniformLocation("Camera"), uniformBinding)
                       .withSampler2D(0, samplerBinding)
                       .build();

    posBuffer = renderer.makeBuffer<glm::vec2>(BUFFER_SIZE);
    uvBuffer = renderer.makeBuffer<glm::vec2>(BUFFER_SIZE);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    pipeline.bindBuffer(posBinding, posBuffer);
    pipeline.bindBuffer(uvBinding, uvBuffer);
}

void EntityRenderLayer::render () {
    pipeline.bindUniform(uniformBinding, uniformBuffer);
    pipeline.bindSampler(samplerBinding, atlas->getTexture());

    pipeline.render(posBuffer.size());
}

void EntityRenderLayer::preRender (phenyl::component::ComponentManager& manager, const Camera& camera) {
    PHENYL_DASSERT(atlas);
    bool reloadNeeded = atlas->rebuild();

    manager.query<Sprite2D>().each([reloadNeeded] (auto entity, Sprite2D& sprite) {
        if (reloadNeeded || !sprite.updated) {
            sprite.update();
        }
    });

    bufferData(manager, camera);
}

void EntityRenderLayer::bufferData (const phenyl::component::ComponentManager& manager, const Camera& camera) {
    posBuffer.clear();
    uvBuffer.clear();

    bufferPosData(manager, posBuffer);
    bufferUvData(manager, uvBuffer);

    posBuffer.upload();
    uvBuffer.upload();

    uniformBuffer->camera = camera.getCamMatrix();
}

static void bufferPosData (const phenyl::component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer) {
    static glm::vec2 vertices[] =  {
            {-1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f}
    };

    manager.query<phenyl::common::GlobalTransform2D, Sprite2D>().each([&buffer] (auto info, const phenyl::common::GlobalTransform2D& transform, const Sprite2D& sprite) {
        for (auto i : vertices) {
            buffer.emplace(transform.transform2D.apply(i));
        }
    });
}

static void bufferUvData (const phenyl::component::EntityComponentManager& manager, Buffer<glm::vec2>& buffer) {
    manager.query<phenyl::common::GlobalTransform2D, Sprite2D>().each([&buffer] (auto info, const phenyl::common::GlobalTransform2D& transform, const Sprite2D& sprite) {
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