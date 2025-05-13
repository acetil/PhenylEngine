#pragma once

#include "graphics/backend/shader.h"
#include "vulkan/vulkan_headers.h"

#include <unordered_map>

namespace spirv_cross {
class Compiler;
struct ShaderResources;
} // namespace spirv_cross

namespace phenyl::vulkan {
class ShaderReflection {
public:
    struct VertexInput {
        std::uint32_t location;
        graphics::ShaderDataType type;
    };

    struct FragmentOutput {
        std::string name;
        std::uint32_t location;
    };

    struct UniformBlock {
        std::size_t size;
        std::uint32_t set;
        std::uint32_t location;

        std::unordered_map<std::string, std::size_t> memberOffsets;

        bool operator== (const UniformBlock&) const = default;
    };

    struct Sampler {
        std::uint32_t set;
        std::uint32_t location;

        bool operator== (const Sampler&) const = default;
    };

    explicit ShaderReflection (
        const std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>>& sources);

    const VertexInput* getAttrib (const std::string& name) const noexcept;
    const FragmentOutput* getOutput (const std::string& name) const noexcept;
    const UniformBlock* getUniformBlock (const std::string& name) const noexcept;
    const Sampler* getSampler (const std::string& name) const noexcept;

private:
    std::unordered_map<std::string, VertexInput> m_attribs;
    std::vector<FragmentOutput> m_outputs;
    std::unordered_map<std::string, UniformBlock> m_uniformBlocks;
    std::unordered_map<std::string, Sampler> m_samplers;

    void addAttribs (const spirv_cross::Compiler& compiler, const spirv_cross::ShaderResources& resources);
    void addOutputs (const spirv_cross::Compiler& compiler, const spirv_cross::ShaderResources& resources);
    void addUniformBlocks (const spirv_cross::Compiler& compiler, const spirv_cross::ShaderResources& resources);
    void addSamplers (const spirv_cross::Compiler& compiler, const spirv_cross::ShaderResources& resources);
};
} // namespace phenyl::vulkan
