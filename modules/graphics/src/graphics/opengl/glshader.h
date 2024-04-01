#pragma once

#include <unordered_set>

#include "common/assets/asset_manager.h"
#include "util/map.h"

#include "graphics/graphics_headers.h"
#include "graphics/renderers/shader.h"

namespace phenyl::graphics {
    class GlShader : public IShader {
    private:
        GLuint programId{0};
        std::unordered_map<std::string, unsigned int> uniformBlocks;
        std::unordered_map<std::string, unsigned int> samplers;

        bool addUniformBlock (const std::string& uniform);
        bool addSampler (const std::string& sampler);

        friend class Builder;
    public:
        class Builder {
        private:
            std::unordered_map<ShaderSourceType, GLuint> shaderSources;
            std::unordered_set<std::string> uniformBlocks;
            std::unordered_set<std::string> samplers;

            friend class GlShader;
        public:
            Builder() = default;

            Builder& withSource (ShaderSourceType type, std::string source);
            Builder& withUniformBlock (std::string uniformName);
            Builder& withSampler (std::string samplerName);
            std::unique_ptr<GlShader> build ();
        };

        explicit GlShader (GLuint programId);
        GlShader (const GlShader&) = delete;
        GlShader (GlShader&& other) noexcept;

        GlShader& operator= (const GlShader&) = delete;
        GlShader& operator= (GlShader&& other) noexcept;

        ~GlShader() override;

        std::size_t hash () const noexcept override;
        std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override;
        std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override;

        void bind ();
    };

    class GlShaderManager : public common::AssetManager<Shader> {
    private:
        std::unordered_map<std::size_t, Shader> shaders;
    public:
        const char* getFileType() const override;
        Shader* load (std::istream& data, std::size_t id) override;
        Shader* load (Shader&& obj, std::size_t id) override;
        void queueUnload(std::size_t id) override;

        void selfRegister ();
    };
}
