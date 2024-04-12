#include "entity_layer.h"
#include "common/assets/assets.h"
#include "common/components/2d/global_transform.h"
#include "graphics/renderer.h"
#include "graphics/components/2d/sprite.h"

#define MAX_ENTITIES 512
#define BUFFER_SIZE (MAX_ENTITIES * 2 * 6)

using namespace phenyl::graphics;

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
}

void EntityRenderLayer::preRender (phenyl::component::ComponentManager& manager, const Camera& camera) {
    bufferData(manager, camera);
}

void EntityRenderLayer::bufferData (const phenyl::component::ComponentManager& manager, const Camera& camera) {
    static glm::vec2 vertices[] =  {
        {-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f}
    };

    vertexBuffer.clear();
    indices.clear();
    samplerStartIndices.clear();
    samplerRenders.clear();

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
