#pragma once

#include "pipeline.h"
#include "renderer.h"
#include "util/map.h"

#include "shader.h"
#include "core/assets/asset.h"
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

    class Material : public core::IAssetType<Material> {
    public:
        struct MatPipeline;
    private:
        Renderer& renderer;
        std::uint32_t materialId;
        core::Asset<Shader> shader;

        util::HashMap<std::uint64_t, MatPipeline> pipelines;
        MaterialProperties materialProperties;
    public:
        struct MatPipeline {
            Pipeline pipeline;

            UniformBinding globalUniform;
            BufferBinding modelBinding;
            std::vector<BufferBinding> streamBindings;

            UniformBinding instanceBinding;
            std::vector<SamplerBinding> samplerBindings;
        };

        Material (Renderer& renderer, std::uint32_t id, core::Asset<Shader> shader, MaterialProperties properties);

        std::uint32_t id () const noexcept {
            return materialId;
        }

        MatPipeline& getPipeline (const MeshLayout& layout);

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
