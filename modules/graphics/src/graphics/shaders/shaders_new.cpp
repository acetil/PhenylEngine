#include "graphics/shaders/shader_new.h"
#include "graphics/shaders/renderer_shader.h"

using namespace graphics;

const char* graphics::getUniformTypeName (UniformType type) {
    switch (type) {
        case UniformType::FLOAT:
            return "float";
        case UniformType::INT:
            return "int";
        case UniformType::VEC2F:
            return "vec2f";
        case UniformType::VEC3F:
            return "vec3f";
        case UniformType::VEC4F:
            return "vec4f";
        case UniformType::MAT2F:
            return "mat2f";
        case UniformType::MAT3F:
            return "mat3f";
        case UniformType::MAT4F:
            return "mat4f";
        default:
            return "<unspecified>";
    }
}

graphics::ShaderProgramNew::~ShaderProgramNew () = default;

void ShaderProgramNew::applyUniform (const std::string& uniform, UniformType uniformType, const unsigned char* dataPtr) {
    if (debugCheckShared()) {
        internal->applyUniform(uniform, uniformType, dataPtr);
    }
}

void ShaderProgramNew::bind () {
    if (debugCheckShared()) {
        internal->bind();
    }
}
