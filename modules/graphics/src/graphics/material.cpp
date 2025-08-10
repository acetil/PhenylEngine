#include "graphics/material.h"

#include "core/assets/assets.h"
#include "renderlayer/mesh_layer.h"

#include <utility>

using namespace phenyl::graphics;

MeshRenderBuilder::MeshRenderBuilder (MaterialRenderPipeline& pipeline, const Mesh& mesh) :
    m_matPipeline{pipeline},
    m_mesh{mesh} {}

MeshRenderBuilder& MeshRenderBuilder::bindModelBuffer (const Buffer<glm::mat4>& buffer, std::size_t offset) {
    m_matPipeline.pipeline.bindBuffer(m_matPipeline.modelBinding, buffer, offset);
    return *this;
}

MeshRenderBuilder& MeshRenderBuilder::bindSampler (MaterialSampler samplerType, ISampler& sampler) {
    if (const auto it = m_matPipeline.samplerBindings.find(samplerType); it != m_matPipeline.samplerBindings.end()) {
        pipeline().bindSampler(it->second, sampler);
    }
    return *this;
}

MeshRenderBuilder& MeshRenderBuilder::bindInstanceUniform (const RawUniformBuffer& instanceUniform) {
    if (m_matPipeline.instanceBindings) {
        pipeline().bindUniform(m_matPipeline.instanceBindings->dataUniformBinding, instanceUniform);
    }
    return *this;
}

void MeshRenderBuilder::render (CommandList& cmdList, FrameBuffer& fb, std::size_t numInstances) {
    m_mesh.bind(pipeline(), m_matPipeline.streamBindings);
    pipeline().renderInstanced(cmdList, fb, numInstances, m_mesh.numVertices());
}

std::optional<UniformBinding> MeshRenderBuilder::getUniformBinding (MaterialRenderUniform uniformType) const noexcept {
    const auto it = m_matPipeline.uniformBindings.find(uniformType);
    return it != m_matPipeline.uniformBindings.end() ? std::optional{it->second} : std::nullopt;
}

Material::Material (Renderer& renderer, std::uint32_t id, std::shared_ptr<Shader> shader,
    MaterialProperties properties) :
    m_renderer{renderer},
    m_id{id},
    m_shader{std::move(shader)},
    materialProperties{std::move(properties)} {}

std::shared_ptr<MaterialInstance> Material::instance () {
    return std::make_shared<MaterialInstance>(m_renderer, shared_from_this(), materialProperties);
}

MeshRenderBuilder Material::renderMesh (ForwardRenderStage renderStage, const Mesh& mesh) {
    return MeshRenderBuilder{getPipeline(renderStage, mesh.layout()), mesh};
}

MaterialRenderPipeline& Material::getPipeline (ForwardRenderStage renderStage, const MeshLayout& layout) {
    auto& pipelineMap = m_pipelines2[layout.layoutId];
    if (auto pipelineIt = pipelineMap.find(renderStage); pipelineIt != pipelineMap.end()) {
        return pipelineIt->second;
    }

    switch (renderStage) {
    case ForwardRenderStage::DEPTH_PREPASS: {
        auto [it, _] = pipelineMap.emplace(renderStage, makeDepthPipeline(layout));
        return it->second;
    }
    case ForwardRenderStage::SHADOW_MAP: {
        auto [it, _] = pipelineMap.emplace(renderStage, makeShadowMapPipeline(layout));
        return it->second;
    }
    case ForwardRenderStage::RENDER: {
        auto [it, _] = pipelineMap.emplace(renderStage, makeRenderPipeline(layout));
        return it->second;
    }
    }
    PHENYL_ABORT("Unexpected ForwardRenderStage value: {}", static_cast<std::uint32_t>(renderStage));
}

MaterialRenderPipeline Material::makeRenderPipeline (const MeshLayout& layout) {
    auto builder = m_renderer.buildPipeline();

    UniformBinding globalUniform;
    UniformBinding lightUniform;
    SamplerBinding shadowMapBinding;
    BufferBinding model;
    builder.withShader(m_shader)
        .withBlending(BlendMode::ADDITIVE)
        .withDepthTesting(false)
        .withUniform<MeshGlobalUniform>(m_shader->uniformLocation("GlobalUniform").value(), globalUniform)
        .withUniform<BPLightUniform>(m_shader->uniformLocation("BPLightUniform").value(), lightUniform)
        .withSampler2D(m_shader->samplerLocation("ShadowMap").value(), shadowMapBinding)
        .withBuffer<glm::mat4>(model, BufferInputRate::INSTANCE);

    // TODO: material specific uniform binding
    // TODO: material specific sampler bindings

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        if (auto loc = m_shader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(loc.value(), streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(m_shader->attribLocation("model").value(), model);

    UniformBinding instanceBinding;
    builder.withRawUniform(m_shader->uniformLocation("Material").value(), instanceBinding);
    return MaterialRenderPipeline{
      .pipeline = builder.build(),
      .streamBindings = std::move(streamBindings),
      .modelBinding = model,
      .uniformBindings = {{MaterialRenderUniform::GLOBAL_UNIFORM, globalUniform},
        {MaterialRenderUniform::BP_LIGHT_UNIFORM, lightUniform}},
      .samplerBindings = {{MaterialSampler::SHADOW_MAP, shadowMapBinding}},
      .instanceBindings =
          MaterialInstanceBindings{
            .dataUniformBinding = instanceBinding,
          },
    };
}

MaterialRenderPipeline Material::makeDepthPipeline (const MeshLayout& layout) {
    auto builder = m_renderer.buildPipeline();
    builder.withBlending(BlendMode::ADDITIVE);

    auto prepassShader = core::Assets::Load<Shader>("phenyl/shaders/mesh_prepass");

    UniformBinding globalUniform;
    BufferBinding model;
    builder.withShader(prepassShader)
        .withDepthTesting()
        .withCulling(CullMode::BACK_FACE)
        .withUniform<MeshGlobalUniform>(prepassShader->uniformLocation("GlobalUniform").value(), globalUniform)
        .withBuffer<glm::mat4>(model, BufferInputRate::INSTANCE);

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        if (auto loc = prepassShader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(loc.value(), streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(prepassShader->attribLocation("model").value(), model);
    return MaterialRenderPipeline{
      .pipeline = builder.build(),
      .streamBindings = std::move(streamBindings),
      .modelBinding = model,
      .uniformBindings = {{MaterialRenderUniform::GLOBAL_UNIFORM, globalUniform}},
    };
}

MaterialRenderPipeline Material::makeShadowMapPipeline (const MeshLayout& layout) {
    auto builder = m_renderer.buildPipeline();

    auto shadowMapShader = core::Assets::Load<Shader>("phenyl/shaders/shadow_map");

    UniformBinding lightUniform;
    BufferBinding model;
    builder.withShader(shadowMapShader)
        .withDepthTesting()
        .withCulling(CullMode::FRONT_FACE)
        .withUniform<BPLightUniform>(shadowMapShader->uniformLocation("BPLightUniform").value(), lightUniform)
        .withBuffer<glm::mat4>(model, BufferInputRate::INSTANCE);

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        if (auto loc = shadowMapShader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(loc.value(), streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(shadowMapShader->attribLocation("model").value(), model);
    return MaterialRenderPipeline{
      .pipeline = builder.build(),
      .streamBindings = std::move(streamBindings),
      .modelBinding = model,
      .uniformBindings = {{MaterialRenderUniform::BP_LIGHT_UNIFORM, lightUniform}},
    };
}

MaterialInstance::MaterialInstance (Renderer& renderer, std::shared_ptr<Material> material,
    const MaterialProperties& properties) :
    m_material{std::move(material)} {
    m_data = renderer.makeRawUniformBuffer(properties.uniformBlockSize);

    for (const auto& [id, type, offset] : properties.uniforms) {
        m_uniforms[id] = MaterialUniform{
          .type = type,
          .offset = offset,
        };
    }
}

void MaterialInstance::upload () {
    m_data.upload();
}

void MaterialInstance::bind (Material::MatPipeline& pipeline) {
    pipeline.pipeline.bindUniform(pipeline.instanceBinding, m_data);
}

MeshRenderBuilder MaterialInstance::render (ForwardRenderStage renderStage, const Mesh& mesh) {
    auto builder = m_material->renderMesh(renderStage, mesh);
    builder.bindInstanceUniform(m_data);
    return builder;
}
