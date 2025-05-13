#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/shader.h"
#include "vk_compiler.h"
#include "vk_shader_reflection.h"
#include "vulkan/vulkan_headers.h"

#include <map>

namespace phenyl::vulkan {
class VulkanShader : public graphics::IShader {
public:
    static std::unique_ptr<VulkanShader> Make (VkDevice device,
        const std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>>& sources,
        const std::vector<std::pair<graphics::ShaderDataType, std::string>>& attribs,
        std::unordered_map<std::string, unsigned int> uniforms);

    ~VulkanShader () override;

    std::vector<VkPipelineShaderStageCreateInfo> getStageInfo ();

    std::size_t hash () const noexcept override;
    std::optional<unsigned int> getAttribLocation (const std::string& attrib) const noexcept override;
    std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override;
    std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override;

    std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock,
        const std::string& uniform) const noexcept override;
    std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept override;

private:
    VkDevice m_device;
    std::unordered_map<graphics::ShaderSourceType, VkShaderModule> m_modules;

    ShaderReflection m_reflection;

    VulkanShader (VkDevice device, std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules,
        ShaderReflection reflection);
};

class VulkanShaderManager : public core::AssetManager<graphics::Shader> {
public:
    VulkanShaderManager (VkDevice device);

    graphics::Shader* load (std::ifstream& data, std::size_t id) override;
    graphics::Shader* load (graphics::Shader&& obj, std::size_t id) override;
    void queueUnload (std::size_t id) override;
    bool isBinary () const override;
    const char* getFileType () const override;

    void selfRegister ();

    void loadDefaultShaders ();

private:
    VkDevice m_device;
    VulkanShaderCompiler m_compiler;

    std::unordered_map<std::size_t, std::unique_ptr<graphics::Shader>> m_shaders;

    std::vector<core::Asset<graphics::Shader>> m_defaultShaders;
    void loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader);

    class Builder {
    public:
        explicit Builder (VkDevice device, const VulkanShaderCompiler& compiler) :
            m_device{device},
            m_compiler{compiler} {}

        Builder& withSource (graphics::ShaderSourceType type, const std::string& source);
        Builder& withAttrib (graphics::ShaderDataType type, std::string attribName);
        Builder& withUniform (std::string attribName, unsigned int location);
        std::unique_ptr<VulkanShader> build ();

    private:
        VkDevice m_device;
        const VulkanShaderCompiler& m_compiler;
        std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>> m_sources;
        std::vector<std::pair<graphics::ShaderDataType, std::string>> m_attribs;
        std::unordered_map<std::string, unsigned int> m_uniformBindings;
    };

    Builder builder ();
};
} // namespace phenyl::vulkan
