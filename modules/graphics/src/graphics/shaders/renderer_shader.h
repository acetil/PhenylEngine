#pragma once

#include <string>

#include "graphics/shaders/shader_new.h"

namespace graphics {
    class RendererShaderProgram {
    public:
        virtual ~RendererShaderProgram() = default;
        virtual void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const unsigned char* uniformPtr) = 0;

        virtual void bind () = 0;
    };
}