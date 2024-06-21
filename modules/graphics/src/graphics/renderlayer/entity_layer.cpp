#include "entity_layer.h"
#include "common/assets/assets.h"
#include "common/components/2d/global_transform.h"
#include "graphics/renderer.h"
#include "graphics/components/2d/sprite.h"
#include "runtime/runtime.h"

#define MAX_ENTITIES 512
#define BUFFER_SIZE (MAX_ENTITIES * 2 * 6)

using namespace phenyl::graphics;

struct EntityRenderData2D : public phenyl::runtime::IResource {
    explicit EntityRenderData2D (EntityRenderLayer& layer) : layer{layer} {}

    EntityRenderLayer& layer;

    [[nodiscard]] std::string_view getName () const noexcept override {
        return "EntityRenderData2D";
    }
};

static void PushEntitySystem (const phenyl::runtime::Resources<EntityRenderData2D>& resources, const phenyl::common::GlobalTransform2D& transform, const Sprite2D& sprite) {
    auto& [data] = resources;
    data.layer.pushEntity(transform, sprite);
}

static void BufferEntitiesSystem (const phenyl::runtime::Resources<EntityRenderData2D, const Camera>& resources) {
    auto& [data, camera] = resources;
    data.layer.bufferEntities(camera);
}

EntityRenderLayer::EntityRenderLayer () : AbstractRenderLayer{0} {}

std::string_view EntityRenderLayer::getName () const {
    return "EntityRenderLayer";
}

void EntityRenderLayer::init (Renderer& renderer) {
    BufferBinding vertexBinding;
    auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/sprite");
    pipeline = renderer.buildPipeline()
           .withShader(shader)
           .withBuffer<Vertex>(vertexBinding)
           .withAttrib<glm::vec2>(0, vertexBinding, offsetof(Vertex, pos))
           .withAttrib<glm::vec2>(1, vertexBinding, offsetof(Vertex, uv))
           .withUniform<Uniform>(*shader->uniformLocation("Camera"), uniformBinding)
           .withSampler2D(*shader->samplerLocation("textureSampler"), samplerBinding)
           .build();

    vertexBuffer = renderer.makeBuffer<Vertex>(BUFFER_SIZE);
    indices = renderer.makeBuffer<std::uint16_t>(BUFFER_SIZE);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    pipeline.bindIndexBuffer(indices);
    pipeline.bindBuffer(vertexBinding, vertexBuffer);
}

void EntityRenderLayer::render () {
    pipeline.bindUniform(uniformBinding, uniformBuffer);

    for (const auto& [off, size, sampler] : samplerRenders) {
        pipeline.bindSampler(samplerBinding, *sampler);
        pipeline.render(size, off);
    }

    vertexBuffer.clear();
    indices.clear();
    samplerStartIndices.clear();
    samplerRenders.clear();
}

void EntityRenderLayer::preRender (phenyl::component::ComponentManager& manager, const Camera& camera) {
    bufferData(manager, camera);
}

void EntityRenderLayer::pushEntity (const common::GlobalTransform2D& transform, const Sprite2D& sprite) {
    if (!sprite.texture) {
        return;
    }

    auto startIndex = vertexBuffer.emplace(Vertex{
        .pos = transform.transform2D.apply({-1.0f, 1.0f}),
        .uv = sprite.uvStart
    });
    vertexBuffer.emplace(Vertex{
        .pos = transform.transform2D.apply({1.0f, 1.0f}),
        .uv = glm::vec2{sprite.uvEnd.x, sprite.uvStart.y}
    });
    vertexBuffer.emplace(Vertex{
        .pos = transform.transform2D.apply({1.0f, -1.0f}),
        .uv = sprite.uvEnd
    });
    vertexBuffer.emplace(Vertex{
        .pos = transform.transform2D.apply({-1.0f, -1.0f}),
        .uv = glm::vec2{sprite.uvStart.x, sprite.uvEnd.y}
    });

    samplerStartIndices.emplace_back(&sprite.texture->sampler(), startIndex);
}

void EntityRenderLayer::bufferEntities (const Camera& camera) {
    std::sort(samplerStartIndices.begin(), samplerStartIndices.end());
    std::uint16_t offset = 0;
    const ISampler* currSampler = nullptr;
    for (const auto& [sampler, startIndex] : samplerStartIndices) {
        if (sampler != currSampler) {
            samplerRenders.emplace_back(SamplerRender{
                .indexOffset = offset,
                .size = 0,
                .sampler = sampler
            });
            currSampler = sampler;
        }

        indices.emplace(startIndex + 0);
        indices.emplace(startIndex + 1);
        indices.emplace(startIndex + 2);

        indices.emplace(startIndex + 0);
        indices.emplace(startIndex + 2);
        indices.emplace(startIndex + 3);

        offset += 6;
        samplerRenders.back().size += 6;
    }

    vertexBuffer.upload();
    indices.upload();

    uniformBuffer->camera = camera.getCamMatrix();
}


void EntityRenderLayer::bufferData (const phenyl::component::ComponentManager& manager, const Camera& camera) {

    manager.query<phenyl::common::GlobalTransform2D, Sprite2D>().each([&] (auto _, const phenyl::common::GlobalTransform2D& transform, const Sprite2D& sprite) {
        if (!sprite.texture) {
            return;
        }

        auto startIndex = vertexBuffer.emplace(Vertex{
            .pos = transform.transform2D.apply({-1.0f, 1.0f}),
            .uv = sprite.uvStart
        });
        vertexBuffer.emplace(Vertex{
            .pos = transform.transform2D.apply({1.0f, 1.0f}),
            .uv = glm::vec2{sprite.uvEnd.x, sprite.uvStart.y}
        });
        vertexBuffer.emplace(Vertex{
            .pos = transform.transform2D.apply({1.0f, -1.0f}),
            .uv = sprite.uvEnd
        });
        vertexBuffer.emplace(Vertex{
            .pos = transform.transform2D.apply({-1.0f, -1.0f}),
            .uv = glm::vec2{sprite.uvStart.x, sprite.uvEnd.y}
        });

        samplerStartIndices.emplace_back(&sprite.texture->sampler(), startIndex);
    });

    std::sort(samplerStartIndices.begin(), samplerStartIndices.end());
    std::uint16_t offset = 0;
    const ISampler* currSampler = nullptr;
    for (const auto& [sampler, startIndex] : samplerStartIndices) {
        if (sampler != currSampler) {
            samplerRenders.emplace_back(SamplerRender{
                .indexOffset = offset,
                .size = 0,
                .sampler = sampler
            });
            currSampler = sampler;
        }

        indices.emplace(startIndex + 0);
        indices.emplace(startIndex + 1);
        indices.emplace(startIndex + 2);

        indices.emplace(startIndex + 0);
        indices.emplace(startIndex + 2);
        indices.emplace(startIndex + 3);

        offset += 6;
        samplerRenders.back().size += 6;
    }

    vertexBuffer.upload();
    indices.upload();

    uniformBuffer->camera = camera.getCamMatrix();
}

void EntityRenderLayer::addSystems (runtime::PhenylRuntime& runtime) {
    runtime.addResource<EntityRenderData2D>(*this);

    runtime.addSystem<phenyl::runtime::Render>(PushEntitySystem);
    runtime.addSystem<phenyl::runtime::Render>(BufferEntitiesSystem);
}
