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
        GLuint programId;
        util::Map<std::string, GLUniform> uniformMap;

        void initShaders (util::Map<ShaderType, std::string>& shaders);
        void applyUniform (GLUniform uniform, const unsigned char* uniformPtr);

    public:
        explicit GLShaderProgram (ShaderBuilder& builder);
        ~GLShaderProgram() override;

        void applyUniform(const std::string &uniformName, ShaderDataType uniformType, const unsigned char *uniformPtr) override;
        void bind() override;
    };

    class GLShaderManager : public common::AssetManager<Shader> {
    private:
        util::Map<std::size_t,  std::unique_ptr<Shader>> shaders;
        GLRenderer* renderer;
    protected:
        Shader* load (std::istream &data, std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;
        void queueUnload (std::size_t id) override;
    public:
        explicit GLShaderManager (GLRenderer* renderer);
        void selfRegister ();
    };
}