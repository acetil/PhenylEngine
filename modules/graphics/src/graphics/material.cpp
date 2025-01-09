#include "graphics/material.h"

#include <utility>

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
    BufferBinding model;
    builder.withShader(shader)
        .withUniform<MeshGlobalUniform>(*shader->uniformLocation("GlobalUniform"), globalUniform)
        .withUniform<BPLightUniform>(*shader->uniformLocation("BPLightUniform"), lightUniform)
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
        .samplerBindings = {} // TODO
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


