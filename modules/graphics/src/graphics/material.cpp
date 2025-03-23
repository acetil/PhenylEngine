#include "graphics/material.h"

#include <utility>

#include "core/assets/assets.h"
#include "renderlayer/mesh_layer.h"

using namespace phenyl::graphics;

Material::Material (Renderer& renderer, std::uint32_t id, core::Asset<Shader> shader, MaterialProperties properties) : renderer{renderer}, materialId{id}, shader{std::move(shader)}, materialProperties{std::move(properties)} {}

Material::MatPipeline& Material::getPipeline (const MeshLayout& layout) {
    if (auto it = pipelines.find(layout.layoutId); it != pipelines.end()) {
        return it->second;
    }

    auto builder = renderer.buildPipeline();

    UniformBinding globalUniform;
    UniformBinding lightUniform;
    SamplerBinding shadowMapBinding;
    BufferBinding model;
    builder.withShader(shader)
        .withBlending(BlendMode::ADDITIVE)
        .withUniform<MeshGlobalUniform>(*shader->uniformLocation("GlobalUniform"), globalUniform)
        .withUniform<BPLightUniform>(*shader->uniformLocation("BPLightUniform"), lightUniform)
        .withSampler2D(*shader->samplerLocation("ShadowMap"), shadowMapBinding)
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
        if (auto loc = shader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(*loc, streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(*shader->attribLocation("model"), model);

    UniformBinding instanceBinding;
    builder.withRawUniform(*shader->uniformLocation("Material"), instanceBinding);

    auto [it, _] = pipelines.emplace(layout.layoutId, MatPipeline{
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
    if (auto it = depthPipelines.find(layout.layoutId); it != depthPipelines.end()) {
        return it->second;
    }

    auto builder = renderer.buildPipeline();

    auto prepassShader = core::Assets::Load<Shader>("phenyl/shaders/mesh_prepass");

    UniformBinding globalUniform;
    BufferBinding model;
    builder.withShader(prepassShader)
        .withUniform<MeshGlobalUniform>(*prepassShader->uniformLocation("GlobalUniform"), globalUniform)
        .withBuffer<glm::mat4>(model, BufferInputRate::INSTANCE);

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        if (auto loc = prepassShader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(*loc, streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(*prepassShader->attribLocation("model"), model);

    auto [it, _] = depthPipelines.emplace(layout.layoutId, DepthPipeline{
        .pipeline = builder.build(),
        .globalUniform = globalUniform,
        .modelBinding = model,
        .streamBindings = std::move(streamBindings)
    });

    return it->second;
}

Material::ShadowMapPipeline& Material::getShadowMapPipeline (const MeshLayout& layout) {
    if (auto it = shadowMapPipelines.find(layout.layoutId); it != shadowMapPipelines.end()) {
        return it->second;
    }

    auto builder = renderer.buildPipeline();

    auto shadowMapShader = core::Assets::Load<Shader>("phenyl/shaders/shadow_map");

    UniformBinding lightUniform;
    BufferBinding model;
    builder.withShader(shadowMapShader)
        .withUniform<BPLightUniform>(*shadowMapShader->uniformLocation("BPLightUniform"), lightUniform)
        .withBuffer<glm::mat4>(model, BufferInputRate::INSTANCE);

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        if (auto loc = shadowMapShader->attribLocation(GetMeshAttribName(i.kind))) {
            builder.withAttrib(*loc, streamBindings[i.stream], i.type, i.offset);
        }
    }

    builder.withAttrib<glm::mat4>(*shadowMapShader->attribLocation("model"), model);

    auto [it, _] = shadowMapPipelines.emplace(layout.layoutId, ShadowMapPipeline{
        .pipeline = builder.build(),
        .lightUniform = lightUniform,
        .modelBinding = model,
        .streamBindings = std::move(streamBindings)
    });

    return it->second;
}

std::shared_ptr<MaterialInstance> Material::instance () {
    return std::make_shared<MaterialInstance>(renderer, assetFromThis(), materialProperties);
}

MaterialInstance::MaterialInstance (Renderer& renderer, core::Asset<Material> material, const MaterialProperties& properties) : instanceMaterial{std::move(material)} {
    instanceData = renderer.makeRawUniformBuffer(properties.uniformBlockSize);

    for (const auto& [id, type, offset] : properties.uniforms) {
        uniforms[id] = MaterialUniform{
            .type = type,
            .offset = offset
        };
    }
}

void MaterialInstance::upload () {
    instanceData.upload();
}

void MaterialInstance::bind (Material::MatPipeline& pipeline) {
    pipeline.pipeline.bindUniform(pipeline.instanceBinding, instanceData);
}


