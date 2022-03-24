#pragma once

#include "graphics/shaders/shader_new.h"
#include "graphics/shaders/renderer_shader.h"
#include "graphics/graphics_headers.h"

#include "util/smart_help.h"

namespace graphics {

    struct GLUniform {
        GLint uniformId;
        ShaderDataType uniformType;
    };

    class GLShaderProgram : public RendererShaderProgram, public util::SmartHelper<GLShaderProgram> {
    private:
        GLuint programId;
        util::Map<std::string, GLUniform> uniformMap;

        void initShaders (util::Map<ShaderType, std::string>& shaders);
        void applyUniform (GLUniform uniform, const unsigned char* uniformPtr);

    public:
        explicit GLShaderProgram (ShaderProgramBuilder& builder);
        ~GLShaderProgram() override;

        void applyUniform(const std::string &uniformName, ShaderDataType uniformType, const unsigned char *uniformPtr) override;
        void bind() override;
    };
}