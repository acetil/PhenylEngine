#pragma once

#include "graphics/backend/buffer.h"
#include "graphics/backend/texture.h"
#include "graphics/backend/uniform_buffer.h"
#include "graphics/maths_headers.h"
#include "mesh.h"

namespace phenyl::graphics {
enum class ForwardRenderStage {
    DEPTH_PREPASS,
    SHADOW_MAP,
    RENDER
};

struct MeshGlobalUniform {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 viewPos;
};

struct BPLightUniform {
    // std140 alignment
    alignas(16) glm::mat4 lightSpace;
    alignas(16) glm::vec3 lightPos;
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec3 lightColor;
    alignas(16) glm::vec3 ambientColor;
    float brightness;
    float cosOuter;
    float cosInner;
    int lightType = 0;
    int castShadows; // TODO: combine with light type
};

struct BPMeshRenderContext {
    const UniformBuffer<MeshGlobalUniform>* globalUniform = nullptr;
    const UniformArrayBuffer<BPLightUniform>* lightUniform = nullptr;
    std::size_t lightIndex = 0;
    const Mesh* mesh = nullptr;
    const Buffer<glm::mat4>* instanceBuffer = nullptr;
    std::size_t instanceOffset = 0;
    std::size_t numInstances = 0;

    ISampler* shadowMap = nullptr;
    ForwardRenderStage stage = ForwardRenderStage::RENDER;
};
} // namespace phenyl::graphics
