#include "vk_shader_reflection.h"

#include <spirv_cross.hpp>

using namespace phenyl::vulkan;
using namespace spirv_cross;

ShaderReflection::ShaderReflection (
    const std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>>& sources) {
    for (const auto& [type, source] : sources) {
        spirv_cross::Compiler compiler{source.data(), source.size()};

        auto res = compiler.get_shader_resources();

        if (type == graphics::ShaderSourceType::VERTEX) {
            addAttribs(compiler, res);
        } else if (type == graphics::ShaderSourceType::FRAGMENT) {
            addOutputs(compiler, res);
        }

        addUniformBlocks(compiler, res);
        addSamplers(compiler, res);
    }
}

void ShaderReflection::addAttribs (const Compiler& compiler, const ShaderResources& resources) {
    for (const auto& i : resources.stage_inputs) {
        auto name = i.name;
        auto location = compiler.get_decoration(i.id, spv::DecorationLocation);

        const auto& baseType = compiler.get_type(i.base_type_id);
        const auto& type = compiler.get_type(i.type_id);

        graphics::ShaderDataType shaderType = graphics::ShaderDataType::UNKNOWN;
        if (type.basetype == SPIRType::BaseType::Short) {
            shaderType = graphics::ShaderDataType::INT16;
        } else if (type.basetype == SPIRType::BaseType::Int) {
            shaderType = graphics::ShaderDataType::INT32;
        } else if (type.basetype == SPIRType::BaseType::Float) {
            auto rows = type.vecsize;
            auto cols = type.columns;

            PHENYL_ASSERT_MSG((cols == 1 || rows == cols) && rows <= 4, "Unimplemented matrix dimensions: {}x{}", rows,
                cols);

            if (cols == 1) {
                if (rows == 1) {
                    shaderType = graphics::ShaderDataType::FLOAT32;
                } else if (rows == 2) {
                    shaderType = graphics::ShaderDataType::VEC2F;
                } else if (rows == 3) {
                    shaderType = graphics::ShaderDataType::VEC3F;
                } else {
                    shaderType = graphics::ShaderDataType::VEC4F;
                }
            } else if (cols == 2) {
                shaderType = graphics::ShaderDataType::MAT2F;
            } else if (cols == 3) {
                shaderType = graphics::ShaderDataType::MAT3F;
            } else {
                shaderType = graphics::ShaderDataType::MAT4F;
            }
        } else {
            PHENYL_ABORT("Unimplemented SPIR-V type: {}", static_cast<std::uint32_t>(type.basetype));
        }

        m_attribs.emplace(name, VertexInput{location, shaderType});
    }
}

void ShaderReflection::addOutputs (const Compiler& compiler, const ShaderResources& resources) {
    for (const auto& i : resources.stage_outputs) {
        auto name = i.name;
        auto location = compiler.get_decoration(i.id, spv::DecorationLocation);
        m_outputs.emplace_back(std::move(name), location);
    }
}

void ShaderReflection::addUniformBlocks (const Compiler& compiler, const ShaderResources& resources) {
    for (const auto& i : resources.uniform_buffers) {
        auto set = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
        auto binding = compiler.get_decoration(i.id, spv::DecorationBinding);

        const auto& type = compiler.get_type(i.type_id);
        auto size = compiler.get_declared_struct_size(type);
        auto numMembers = type.member_types.size();

        std::unordered_map<std::string, std::size_t> memberOffsets;
        for (std::size_t j = 0; j < numMembers; j++) {
            const auto& memberName = compiler.get_member_name(i.base_type_id, j);
            auto memberOff = compiler.type_struct_member_offset(type, j);

            memberOffsets.emplace(memberName, memberOff);
        }

        UniformBlock block{.size = size, .set = set, .location = binding, .memberOffsets = std::move(memberOffsets)};

        if (auto it = m_uniformBlocks.find(i.name); it == m_uniformBlocks.end()) {
            PHENYL_LOGD(PHENYL_LOGGER, "Adding uniform block \"{}\"", i.name);
            m_uniformBlocks.emplace(i.name, std::move(block));
        } else {
            PHENYL_ASSERT_MSG(it->second == block, "Inconsistent uniform block: \"{}\"", i.name);
        }
    }
}

void ShaderReflection::addSamplers (const Compiler& compiler, const ShaderResources& resources) {
    for (const auto& i : resources.sampled_images) {
        auto set = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
        auto binding = compiler.get_decoration(i.id, spv::DecorationBinding);
        PHENYL_LOGD(PHENYL_LOGGER, "Found sampler: {}, binding: {}, set: {}", i.name, binding, set);
        // TODO: types

        Sampler sampler{.set = set, .location = binding};

        if (auto it = m_samplers.find(i.name); it == m_samplers.end()) {
            m_samplers.emplace(i.name, sampler);
        } else {
            PHENYL_ASSERT_MSG(it->second == sampler, "Inconsistent sampler: \"{}\"", i.name);
        }
    }
}

const ShaderReflection::VertexInput* ShaderReflection::getAttrib (const std::string& name) const noexcept {
    auto it = m_attribs.find(name);
    return it != m_attribs.end() ? &it->second : nullptr;
}

const ShaderReflection::FragmentOutput* ShaderReflection::getOutput (const std::string& name) const noexcept {
    auto it = std::ranges::find_if(m_outputs, [&] (const auto& x) { return x.name == name; });
    return it != m_outputs.end() ? &*it : nullptr;
}

const ShaderReflection::UniformBlock* ShaderReflection::getUniformBlock (const std::string& name) const noexcept {
    auto it = m_uniformBlocks.find(name);
    return it != m_uniformBlocks.end() ? &it->second : nullptr;
}

const ShaderReflection::Sampler* ShaderReflection::getSampler (const std::string& name) const noexcept {
    auto it = m_samplers.find(name);
    return it != m_samplers.end() ? &it->second : nullptr;
}
