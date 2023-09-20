#pragma once

#include <string>

#include "graphics/shaders/shaders.h"

namespace phenyl::graphics {
    class RendererShader {
    public:
        virtual ~RendererShader() = default;
        virtual void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const unsigned char* uniformPtr) = 0;

        virtual void bind () = 0;
    };
}