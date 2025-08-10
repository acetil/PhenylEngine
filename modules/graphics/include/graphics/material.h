#pragma once

#include "core/assets/asset.h"
#include "detail/loggers.h"
#include "graphics/backend/pipeline.h"
#include "graphics/backend/renderer.h"
#include "graphics/backend/shader.h"
#include "mesh/blinn_phong.h"
#include "mesh/mesh.h"

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

enum class MaterialRenderUniform {
    GLOBAL_UNIFORM,
    BP_LIGHT_UNIFORM
};

enum class MaterialSampler {
    SHADOW_MAP
};

struct MaterialInstanceBindings {
    UniformBinding dataUniformBinding;
    std::vector<SamplerBinding> samplerBindings;
};

struct MaterialRenderPipeline {
    Pipeline pipeline;
    std::vector<BufferBinding> streamBindings;

    UniformBinding modelBinding;
    std::unordered_map<MaterialRenderUniform, UniformBinding> uniformBindings;
    std::unordered_map<MaterialSampler, SamplerBinding> samplerBindings;

    std::optional<MaterialInstanceBindings> instanceBindings;
};

class MeshRenderBuilder {
public:
    MeshRenderBuilder (MaterialRenderPipeline& pipeline, const Mesh& mesh);

    MeshRenderBuilder& bindModelBuffer (const Buffer<glm::mat4>& buffer, std::size_t offset);
    MeshRenderBuilder& bindSampler (MaterialSampler samplerType, ISampler& sampler);

    template <typename T>
    MeshRenderBuilder& bindUniform (MaterialRenderUniform uniformType, const UniformBuffer<T>& uniform) {
        if (auto binding = getUniformBinding(uniformType)) {
            pipeline().bindUniform(*binding, uniform);
        }
        return *this;
    }

    template <typename T>
    MeshRenderBuilder& bindUniform (MaterialRenderUniform uniformType, const UniformArrayBuffer<T>& uniformArray,
        std::size_t index) {
        if (auto binding = getUniformBinding(uniformType)) {
            pipeline().bindUniform(*binding, uniformArray, index);
        }
        return *this;
    }

    MeshRenderBuilder& bindInstanceUniform (const RawUniformBuffer& instanceUniform);

    void render (CommandList& cmdList, FrameBuffer& fb, std::size_t numInstances);

private:
    MaterialRenderPipeline& m_matPipeline;
    const Mesh& m_mesh;

    Pipeline& pipeline () {
        return m_matPipeline.pipeline;
    }

    [[nodiscard]] std::optional<UniformBinding> getUniformBinding (MaterialRenderUniform uniformType) const noexcept;
};

class Material : public core::Asset<Material>, public std::enable_shared_from_this<Material> {
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

    Material (Renderer& renderer, std::uint32_t id, std::shared_ptr<Shader> shader, MaterialProperties properties);

    std::uint32_t id () const noexcept {
        return m_id;
    }

    std::shared_ptr<MaterialInstance> instance ();
    MeshRenderBuilder renderMesh (ForwardRenderStage renderStage, const Mesh& mesh);

private:
    Renderer& m_renderer;
    std::uint32_t m_id;
    std::shared_ptr<Shader> m_shader;

    util::HashMap<std::uint64_t, std::unordered_map<ForwardRenderStage, MaterialRenderPipeline>> m_pipelines2;

    MaterialProperties materialProperties;

    MaterialRenderPipeline& getPipeline (ForwardRenderStage renderStage, const MeshLayout& layout);

    MaterialRenderPipeline makeRenderPipeline (const MeshLayout& layout);
    MaterialRenderPipeline makeDepthPipeline (const MeshLayout& layout);
    MaterialRenderPipeline makeShadowMapPipeline (const MeshLayout& layout);
};

class MaterialInstance : public core::Asset<MaterialInstance> {
public:
    MaterialInstance (Renderer& renderer, std::shared_ptr<Material> material, const MaterialProperties& properties);

    template <typename T>
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

    MeshRenderBuilder render (ForwardRenderStage renderStage, const Mesh& mesh);

private:
    struct MaterialUniform {
        ShaderDataType type;
        std::size_t offset;
    };

    std::shared_ptr<Material> m_material;
    RawUniformBuffer m_data;
    std::unordered_map<std::string, MaterialUniform> m_uniforms;
};
} // namespace phenyl::graphics
