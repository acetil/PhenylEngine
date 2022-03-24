#include "graphics/shaders/shader_new.h"
#include "graphics/shaders/renderer_shader.h"

using namespace graphics;

const char* graphics::getUniformTypeName (ShaderDataType type) {
    switch (type) {
        case ShaderDataType::FLOAT:
            return "float";
        case ShaderDataType::INT:
            return "int";
        case ShaderDataType::VEC2F:
            return "vec2f";
        case ShaderDataType::VEC3F:
            return "vec3f";
        case ShaderDataType::VEC4F:
            return "vec4f";
        case ShaderDataType::MAT2F:
            return "mat2f";
        case ShaderDataType::MAT3F:
            return "mat3f";
        case ShaderDataType::MAT4F:
            return "mat4f";
        case ShaderDataType::UNKNOWN:
            return "<unknown>";
        default:
            return "<unspecified>";
    }
}

graphics::ShaderProgramNew::~ShaderProgramNew () = default;

void ShaderProgramNew::applyUniform (const std::string& uniform, ShaderDataType uniformType, const unsigned char* dataPtr) {
    if (debugCheckShared()) {
        internal->applyUniform(uniform, uniformType, dataPtr);
    }
}

void ShaderProgramNew::bind () {
    if (debugCheckShared()) {
        internal->bind();
    }
}
