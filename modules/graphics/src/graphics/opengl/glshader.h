#pragma once

#include "graphics/shaders/shaders.h"
#include "graphics/shaders/renderer_shader.h"
#include "graphics/graphics_headers.h"
#include "common/assets/asset_manager.h"

#include "util/map.h"
#include "util/smart_help.h"

namespace phenyl::graphics {
    class GLRenderer;

    struct GLUniform {
        GLint uniformId;
        ShaderDataType uniformType;
    };

    class GLShaderProgram : public RendererShader, public util::SmartHelper<GLShaderProgram> {
    private:
        GLuint programId{0};
        util::Map<std::string, GLUniform> uniformMap;
        util::Map<std::string, unsigned int> blockUniformLocations;

        void initShaders (util::Map<ShaderType, std::string>& shaders);
        void initShaderSources (const util::Map<ShaderType, std::string>& sources);

        void applyUniform (GLUniform uniform, const unsigned char* uniformPtr);

    public:
        explicit GLShaderProgram (ShaderBuilder& builder);
        explicit GLShaderProgram (const ShaderSourceSpec& spec);
        ~GLShaderProgram() override;

        void applyUniform(const std::string &uniformName, ShaderDataType uniformType, const unsigned char *uniformPtr) override;
        void bind() override;
        unsigned int getUniformBlockLocation (const std::string& uniform) override;
    };

    class GLShaderManager : public common::AssetManager<Shader> {
    private:
        util::Map<std::size_t,  std::unique_ptr<Shader>> shaders;
        GLRenderer* renderer;
    protected:
        Shader* load (std::istream &data, std::size_t id) override;
        Shader* load (phenyl::graphics::Shader &&obj, std::size_t id) override;

        [[nodiscard]] const char* getFileType () const override;
        void queueUnload (std::size_t id) override;
    public:
        explicit GLShaderManager (GLRenderer* renderer);
        void selfRegister ();
    };
}