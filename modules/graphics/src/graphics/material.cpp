#include "graphics/material.h"

#include "core/assets/assets.h"
#include "renderlayer/mesh_layer.h"

#include <utility>

using namespace phenyl::graphics;

Material::Material (Renderer& renderer, std::uint32_t id, std::shared_ptr<Shader> shader,
    MaterialProperties properties) :
    m_renderer{renderer},
    m_id{id},
    m_shader{std::move(shader)},
    materialProperties{std::move(properties)} {}

Material::MatPipeline& Material::getPipeline (const MeshLayout& layout) {
    if (auto it = m_pipelines.find(layout.layoutId); it != m_pipelines.end()) {
        return it->second;
    }

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

    auto [it, _] = m_pipelines.emplace(layout.layoutId,
        MatPipeline{
          .pipeline = builder.build(),
          .globalUniform = globalUniform,
          .lightUniform = lightUniform,
          .modelBinding = model,
          .streamBindings = std::move(streamBindings),
          .instanceBinding = instanceBinding,
          .shadowMapBinding = shadowMapBinding,
          .samplerBindings = {} // TODO
        });
    return it->second;
}

Material::DepthPipeline& Material::getDepthPipeline (const MeshLayout& layout) {
    if (auto it = m_depthPipelines.find(layout.layoutId); it != m_depthPipelines.end()) {
        return it->second;
    }

    auto builder = m_renderer.buildPipeline();
    builder.withBlending(BlendMode::ADDITIVE);

    auto prepassShader = core::Assets::Load2<Shader>("phenyl/shaders/mesh_prepass");

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

    auto [it, _] = m_depthPipelines.emplace(layout.layoutId,
        DepthPipeline{.pipeline = builder.build(),
          .globalUniform = globalUniform,
          .modelBinding = model,
          .streamBindings = std::move(streamBindings)});

    return it->second;
}

Material::ShadowMapPipeline& Material::getShadowMapPipeline (const MeshLayout& layout) {
    if (auto it = m_shadowMapPipelines.find(layout.layoutId); it != m_shadowMapPipelines.end()) {
        return it->second;
    }

    auto builder = m_renderer.buildPipeline();

    auto shadowMapShader = core::Assets::Load2<Shader>("phenyl/shaders/shadow_map");

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

    auto [it, _] = m_shadowMapPipelines.emplace(layout.layoutId,
        ShadowMapPipeline{
          .pipeline = builder.build(),
          .lightUniform = lightUniform,
          .modelBinding = model,
          .streamBindings = std::move(streamBindings),
        });

    return it->second;
}

std::shared_ptr<MaterialInstance> Material::instance () {
    return std::make_shared<MaterialInstance>(m_renderer, shared_from_this(), materialProperties);
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
