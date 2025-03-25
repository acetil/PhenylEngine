#pragma once

#include "graphics/backend/pipeline.h"
#include "graphics/backend/renderer.h"
#include "util/map.h"

#include "graphics/backend/shader.h"
#include "core/assets/asset.h"
#include "mesh/mesh.h"
#include "detail/loggers.h"

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
        struct MatPipeline;
        struct DepthPipeline;
        struct ShadowMapPipeline;
    private:
        Renderer& renderer;
        std::uint32_t materialId;
        core::Asset<Shader> shader;

        util::HashMap<std::uint64_t, MatPipeline> pipelines;
        util::HashMap<std::uint64_t, DepthPipeline> depthPipelines;
        util::HashMap<std::uint64_t, ShadowMapPipeline> shadowMapPipelines;

        MaterialProperties materialProperties;
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
            return materialId;
        }

        MatPipeline& getPipeline (const MeshLayout& layout);
        DepthPipeline& getDepthPipeline (const MeshLayout& layout);
        ShadowMapPipeline& getShadowMapPipeline (const MeshLayout& layout);

        std::shared_ptr<MaterialInstance> instance ();
    };

    class MaterialInstance {
    private:
        struct MaterialUniform {
            ShaderDataType type;
            std::size_t offset;
        };

        core::Asset<Material> instanceMaterial;
        RawUniformBuffer instanceData;
        std::unordered_map<std::string, MaterialUniform> uniforms;

    public:
        MaterialInstance (Renderer& renderer, core::Asset<Material> material, const MaterialProperties& properties);

        template <typename T>
        void set (const std::string& uniform, T&& val) requires (GetShaderDataType<T>() != ShaderDataType::UNKNOWN) {
            auto it = uniforms.find(uniform);
            if (it == uniforms.end() || it->second.type != GetShaderDataType<T>()) {
                PHENYL_LOGE(detail::GRAPHICS_LOGGER, "Failed to find material uniform {}", uniform);
                return;
            }

            *reinterpret_cast<T*>(instanceData.data() + it->second.offset) = std::forward<T>(val);
        }

        Material* material () noexcept {
            return instanceMaterial.get();
        }

        const Material* material () const noexcept {
            return instanceMaterial.get();
        }

        void upload ();
        void bind (Material::MatPipeline& pipeline);
    };
}
