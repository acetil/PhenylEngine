#include "entity_layer.h"

#include "core/assets/assets.h"
#include "core/components/2d/global_transform.h"
#include "core/runtime.h"
#include "graphics/backend/renderer.h"
#include "graphics/components/2d/sprite.h"

#define MAX_ENTITIES 512
#define BUFFER_SIZE  (MAX_ENTITIES * 2 * 6)

using namespace phenyl::graphics;

struct EntityRenderData2D : public phenyl::core::IResource {
    explicit EntityRenderData2D (EntityRenderLayer& layer) : layer{layer} {}

    EntityRenderLayer& layer;

    [[nodiscard]] std::string_view getName () const noexcept override {
        return "EntityRenderData2D";
    }
};

static void PushEntitySystem (const phenyl::core::Resources<EntityRenderData2D>& resources,
    const phenyl::core::GlobalTransform2D& transform, const Sprite2D& sprite) {
    auto& [data] = resources;
    data.layer.pushEntity(transform, sprite);
}

static void BufferEntitiesSystem (const phenyl::core::Resources<EntityRenderData2D, const Camera2D>& resources) {
    auto& [data, camera] = resources;
    data.layer.bufferEntities(camera);
}

EntityRenderLayer::EntityRenderLayer () : AbstractRenderLayer{0} {}

std::string_view EntityRenderLayer::getName () const {
    return "EntityRenderLayer";
}

void EntityRenderLayer::init (Renderer& renderer) {
    BufferBinding vertexBinding;
    auto shader = phenyl::core::Assets::Load<Shader>("phenyl/shaders/sprite");
    m_pipeline = renderer.buildPipeline()
                     .withShader(shader)
                     .withBuffer<Vertex>(vertexBinding)
                     .withAttrib<glm::vec2>(0, vertexBinding, offsetof(Vertex, pos))
                     .withAttrib<glm::vec2>(1, vertexBinding, offsetof(Vertex, uv))
                     .withUniform<Uniform>(shader->uniformLocation("Camera").value(), m_uniformBinding)
                     .withSampler2D(shader->samplerLocation("textureSampler").value(), m_samplerBinding)
                     .build();

    m_vertexBuffer = renderer.makeBuffer<Vertex>(BUFFER_SIZE, BufferStorageHint::DYNAMIC);
    m_indices = renderer.makeBuffer<std::uint16_t>(BUFFER_SIZE, BufferStorageHint::DYNAMIC, true);
    m_uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    m_pipeline.bindIndexBuffer(m_indices);
    m_pipeline.bindBuffer(vertexBinding, m_vertexBuffer);
}

void EntityRenderLayer::render (Renderer& renderer) {
    auto cmdList = renderer.getCommandList();
    m_pipeline.bindUniform(m_uniformBinding, m_uniformBuffer);

    for (const auto& [off, size, sampler] : m_samplerRenders) {
        m_pipeline.bindSampler(m_samplerBinding, *sampler);
        m_pipeline.render(cmdList, size, off);
    }

    m_vertexBuffer.clear();
    m_indices.clear();
    m_samplerStartIndices.clear();
    m_samplerRenders.clear();
}

void EntityRenderLayer::pushEntity (const core::GlobalTransform2D& transform, const Sprite2D& sprite) {
    if (!sprite.texture) {
        return;
    }

    auto startIndex = m_vertexBuffer.emplace(Vertex{
      .pos = transform.transform * glm::vec2{-1.0f, 1.0f},
      .uv = sprite.uvStart,
    });
    m_vertexBuffer.emplace(Vertex{
      .pos = transform.transform * glm::vec2{1.0f, 1.0f},
      .uv = glm::vec2{sprite.uvEnd.x, sprite.uvStart.y},
    });
    m_vertexBuffer.emplace(Vertex{.pos = transform.transform * glm::vec2{1.0f, -1.0f}, .uv = sprite.uvEnd});
    m_vertexBuffer.emplace(Vertex{.pos = transform.transform * glm::vec2{-1.0f, -1.0f},
      .uv = glm::vec2{sprite.uvStart.x, sprite.uvEnd.y}});

    m_samplerStartIndices.emplace_back(&sprite.texture->sampler(), startIndex);
}

void EntityRenderLayer::bufferEntities (const Camera2D& camera) {
    std::sort(m_samplerStartIndices.begin(), m_samplerStartIndices.end());
    std::uint16_t offset = 0;
    const ISampler* currSampler = nullptr;
    for (const auto& [sampler, startIndex] : m_samplerStartIndices) {
        if (sampler != currSampler) {
            m_samplerRenders.emplace_back(SamplerRender{.indexOffset = offset, .size = 0, .sampler = sampler});
            currSampler = sampler;
        }

        m_indices.emplace(startIndex + 0);
        m_indices.emplace(startIndex + 1);
        m_indices.emplace(startIndex + 2);

        m_indices.emplace(startIndex + 0);
        m_indices.emplace(startIndex + 2);
        m_indices.emplace(startIndex + 3);

        offset += 6;
        m_samplerRenders.back().size += 6;
    }

    m_vertexBuffer.upload();
    m_indices.upload();

    m_uniformBuffer->camera = camera.getCamMatrix();
    m_uniformBuffer.upload();
}

// void EntityRenderLayer::bufferData (phenyl::core::World& world, const Camera2D& camera) {
//
//     world.query<phenyl::core::GlobalTransform2D, Sprite2D>().each([&] (const
//     phenyl::core::GlobalTransform2D& transform, const Sprite2D& sprite) {
//         if (!sprite.texture) {
//             return;
//         }
//
//         auto startIndex = vertexBuffer.emplace(Vertex{
//             .pos = transform.transform2D.apply({-1.0f, 1.0f}),
//             .uv = sprite.uvStart
//         });
//         vertexBuffer.emplace(Vertex{
//             .pos = transform.transform2D.apply({1.0f, 1.0f}),
//             .uv = glm::vec2{sprite.uvEnd.x, sprite.uvStart.y}
//         });
//         vertexBuffer.emplace(Vertex{
//             .pos = transform.transform2D.apply({1.0f, -1.0f}),
//             .uv = sprite.uvEnd
//         });
//         vertexBuffer.emplace(Vertex{
//             .pos = transform.transform2D.apply({-1.0f, -1.0f}),
//             .uv = glm::vec2{sprite.uvStart.x, sprite.uvEnd.y}
//         });
//
//         samplerStartIndices.emplace_back(&sprite.texture->sampler(), startIndex);
//     });
//
//     std::sort(samplerStartIndices.begin(), samplerStartIndices.end());
//     std::uint16_t offset = 0;
//     const ISampler* currSampler = nullptr;
//     for (const auto& [sampler, startIndex] : samplerStartIndices) {
//         if (sampler != currSampler) {
//             samplerRenders.emplace_back(SamplerRender{
//                 .indexOffset = offset,
//                 .size = 0,
//                 .sampler = sampler
//             });
//             currSampler = sampler;
//         }
//
//         indices.emplace(startIndex + 0);
//         indices.emplace(startIndex + 1);
//         indices.emplace(startIndex + 2);
//
//         indices.emplace(startIndex + 0);
//         indices.emplace(startIndex + 2);
//         indices.emplace(startIndex + 3);
//
//         offset += 6;
//         samplerRenders.back().size += 6;
//     }
//
//     vertexBuffer.upload();
//     indices.upload();
//
//     uniformBuffer->camera = camera.getCamMatrix();
//     uniformBuffer.upload();
// }

void EntityRenderLayer::addSystems (core::PhenylRuntime& runtime) {
    runtime.addResource<EntityRenderData2D>(*this);

    runtime.addSystem<phenyl::core::Render>("EntityRender::PushEntity", PushEntitySystem)
        .runBefore(runtime.addSystem<phenyl::core::Render>("EntityRender::BufferEntities", BufferEntitiesSystem));
}
