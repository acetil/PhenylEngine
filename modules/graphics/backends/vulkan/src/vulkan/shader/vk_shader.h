#pragma once

#include "core/assets/asset_manager.h"

#include "graphics/backend/shader.h"
#include "vulkan/vulkan_headers.h"
#include "vk_compiler.h"

namespace phenyl::vulkan {
    class VulkanShader : public graphics::IShader {
    private:
        VkDevice device;
        std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules;

        VulkanShader (VkDevice device, std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules);
    public:
        static std::unique_ptr<VulkanShader> Make (VkDevice device, const std::unordered_map<graphics::ShaderSourceType,
            std::vector<std::uint32_t>>& sources);

        ~VulkanShader();

        std::vector<VkPipelineShaderStageCreateInfo> getStageInfo ();

        std::size_t hash () const noexcept override;
        std::optional<unsigned int> getAttribLocation (const std::string& attrib) const noexcept override;
        std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override;
        std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override;

        std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock, const std::string& uniform) const noexcept override;
        std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept override;
    };

    class VulkanShaderManager : public core::AssetManager<graphics::Shader> {
    private:
        VkDevice device;
        VulkanShaderCompiler compiler;

        std::unordered_map<std::size_t, std::unique_ptr<graphics::Shader>> shaders;

        std::vector<core::Asset<graphics::Shader>> defaultShaders;
        void loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader);

        class Builder {
        private:
            VkDevice device;
            const VulkanShaderCompiler& compiler;
            std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>> shaderSources;

        public:

            explicit Builder (VkDevice device, const VulkanShaderCompiler& compiler) : device{device}, compiler{compiler} {}

            Builder& withSource (graphics::ShaderSourceType type, const std::string& source);
            std::unique_ptr<VulkanShader> build ();
        };

        Builder builder ();
    public:
        VulkanShaderManager (VkDevice device);

        graphics::Shader* load (std::ifstream& data, std::size_t id) override;
        graphics::Shader* load (graphics::Shader&& obj, std::size_t id) override;
        void queueUnload (std::size_t id) override;
        bool isBinary () const override;
        const char* getFileType() const override;

        void selfRegister ();

        void loadDefaultShaders ();
        void clearDefaults ();
    };
}
