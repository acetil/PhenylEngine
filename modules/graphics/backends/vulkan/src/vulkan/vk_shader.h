#pragma once

#include "core/assets/asset_manager.h"

#include "graphics/backend/shader.h"

namespace phenyl::vulkan {
    class VulkanShader : public graphics::IShader {
    public:
        std::size_t hash () const noexcept override;
        std::optional<unsigned int> getAttribLocation (const std::string& attrib) const noexcept override;
        std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override;
        std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override;

        std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock, const std::string& uniform) const noexcept override;
        std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept override;
    };

    class VulkanShaderManager : public core::AssetManager<graphics::Shader> {
    private:
        std::unordered_map<std::size_t, std::unique_ptr<graphics::Shader>> shaders;

        std::vector<core::Asset<graphics::Shader>> defaultShaders;
        void loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader);
    public:
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
