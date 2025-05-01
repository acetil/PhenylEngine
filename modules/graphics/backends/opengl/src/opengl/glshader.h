#pragma once

#include <unordered_set>

#include "core/assets/asset_manager.h"
#include "util/map.h"

#include "opengl_headers.h"
#include "graphics/backend/shader.h"

namespace phenyl::opengl {
    class GlShader : public graphics::IShader {
    public:
        class Builder {
        public:
            Builder() = default;

            Builder& withSource (graphics::ShaderSourceType type, std::string source);
            Builder& withAttrib (graphics::ShaderDataType type, std::string attrib);
            Builder& withUniformBlock (std::string uniformName);
            Builder& withSampler (std::string samplerName);
            std::unique_ptr<GlShader> build ();

        private:
            std::unordered_map<graphics::ShaderSourceType, GLuint> m_sources;
            std::unordered_map<std::string, graphics::ShaderDataType> m_attribs;
            std::unordered_set<std::string> m_uniformBlocks;
            std::unordered_set<std::string> m_samplers;

            friend class GlShader;
        };

        explicit GlShader (GLuint programId);
        GlShader (const GlShader&) = delete;
        GlShader (GlShader&& other) noexcept;

        GlShader& operator= (const GlShader&) = delete;
        GlShader& operator= (GlShader&& other) noexcept;

        ~GlShader() override;

        std::size_t hash () const noexcept override;
        std::optional<unsigned int> getAttribLocation (const std::string& attrib) const noexcept override;
        std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override;
        std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override;

        std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock, const std::string& uniform) const noexcept override;
        std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept override;

        void bind ();

    private:
        GLuint m_program{0};
        std::unordered_map<std::string, unsigned int> m_attribs;
        std::unordered_map<std::string, unsigned int> m_uniformBlocks;
        std::unordered_map<std::string, unsigned int> m_samplers;

        bool addAttrib (graphics::ShaderDataType type, const std::string& attrib);
        bool addUniformBlock (const std::string& uniform);
        bool addSampler (const std::string& sampler);

        friend class Builder;
    };

    class GlShaderManager : public core::AssetManager<graphics::Shader> {
    private:
        std::unordered_map<std::size_t, graphics::Shader> m_shaders;
    public:
        const char* getFileType() const override;
        graphics::Shader* load (std::ifstream& data, std::size_t id) override;
        graphics::Shader* load (graphics::Shader&& obj, std::size_t id) override;
        void queueUnload(std::size_t id) override;

        void selfRegister ();
    };
}
