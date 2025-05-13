#pragma once

#include "core/assets/asset.h"
#include "detail/loggers.h"
#include "graphics/backend/pipeline.h"
#include "graphics/backend/renderer.h"
#include "graphics/backend/shader.h"
#include "mesh/mesh.h"
#include "util/map.h"

namespace phenyl::graphics {
class MaterialInstance;

struct MaterialProperties {
    struct Uniform {
        std::string id;
        ShaderDataType type;
        std::size_t offset;
    };

    std::vector<Uniform> uniforms;
    std::size_t uniformBlockSize;
};

class Material : public core::IAssetType<Material> {
public:
    struct MatPipeline {
        Pipeline pipeline;

        UniformBinding globalUniform;
        UniformBinding lightUniform;
        BufferBinding modelBinding;
        std::vector<BufferBinding> streamBindings;

        UniformBinding instanceBinding;
        SamplerBinding shadowMapBinding;
        std::vector<SamplerBinding> samplerBindings;
    };

    struct DepthPipeline {
        Pipeline pipeline;

        UniformBinding globalUniform;
        BufferBinding modelBinding;
        std::vector<BufferBinding> streamBindings;
    };

    struct ShadowMapPipeline {
        Pipeline pipeline;

        UniformBinding lightUniform;
        BufferBinding modelBinding;
        std::vector<BufferBinding> streamBindings;
    };

    Material (Renderer& renderer, std::uint32_t id, core::Asset<Shader> shader, MaterialProperties properties);

    std::uint32_t id () const noexcept {
        return m_id;
    }

    MatPipeline& getPipeline (const MeshLayout& layout);
    DepthPipeline& getDepthPipeline (const MeshLayout& layout);
    ShadowMapPipeline& getShadowMapPipeline (const MeshLayout& layout);

    std::shared_ptr<MaterialInstance> instance ();

private:
    Renderer& m_renderer;
    std::uint32_t m_id;
    core::Asset<Shader> m_shader;

    util::HashMap<std::uint64_t, MatPipeline> m_pipelines;
    util::HashMap<std::uint64_t, DepthPipeline> m_depthPipelines;
    util::HashMap<std::uint64_t, ShadowMapPipeline> m_shadowMapPipelines;

    MaterialProperties materialProperties;
};

class MaterialInstance {
public:
    MaterialInstance (Renderer& renderer, core::Asset<Material> material, const MaterialProperties& properties);

    template<typename T>
    void set (const std::string& uniform, T&& val) requires (GetShaderDataType<T>() != ShaderDataType::UNKNOWN)
    {
        auto it = m_uniforms.find(uniform);
        if (it == m_uniforms.end() || it->second.type != GetShaderDataType<T>()) {
            PHENYL_LOGE(detail::GRAPHICS_LOGGER, "Failed to find material uniform {}", uniform);
            return;
        }

        *reinterpret_cast<T*>(m_data.data() + it->second.offset) = std::forward<T>(val);
    }

    Material* material () noexcept {
        return m_material.get();
    }

    const Material* material () const noexcept {
        return m_material.get();
    }

    void upload ();
    void bind (Material::MatPipeline& pipeline);

private:
    struct MaterialUniform {
        ShaderDataType type;
        std::size_t offset;
    };

    core::Asset<Material> m_material;
    RawUniformBuffer m_data;
    std::unordered_map<std::string, MaterialUniform> m_uniforms;
};
} // namespace phenyl::graphics
