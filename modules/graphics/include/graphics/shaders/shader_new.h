#pragma once

#include <memory>
#include <utility>
#include "logging/logging.h"
#include "graphics/maths_headers.h"
#include "util/map.h"

namespace graphics {

    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    enum class UniformType {
        FLOAT,
        INT,
        VEC2F,
        VEC3F,
        VEC4F,
        MAT2F,
        MAT3F,
        MAT4F
    };

    const char* getUniformTypeName (UniformType type);

    class RendererShaderProgram;

    class ShaderProgramNew {
    private:
        std::shared_ptr<RendererShaderProgram> internal;
        void applyUniform (const std::string& uniformName, UniformType uniformType, const unsigned char* dataPtr);
        template <typename T>
        void applyUniform (const std::string& uniformName, UniformType uniformType, const T* dataPtr) {
            applyUniform(uniformName, uniformType, (const unsigned char*)dataPtr);
        }
        inline bool debugCheckShared () {
#ifndef NDEBUG
            if (!internal) {
                logging::log(LEVEL_ERROR, "Empty ShaderProgram instance! Engine will crash here in release!");
                return false;
            }
#endif
            return true;
        }
    public:
        ShaderProgramNew () = default;
        explicit ShaderProgramNew (std::shared_ptr<RendererShaderProgram> _internal) : internal{std::move(_internal)} {};
        ~ShaderProgramNew();

        template <typename T>
        void applyUniform (const std::string& uniformName, const T& val) {
            if constexpr (std::is_same_v<T, float>) {
                applyUniform(uniformName, UniformType::FLOAT, &val);
            } else if constexpr (std::is_same_v<T, int>) {
                applyUniform(uniformName, UniformType::INT, &val);
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                applyUniform(uniformName, UniformType::VEC2F, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                applyUniform(uniformName, UniformType::VEC3F, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                applyUniform(uniformName, UniformType::VEC4F, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::mat2>) {
                applyUniform(uniformName, UniformType::MAT2F, &val[0][0]);
            } else if constexpr (std::is_same_v<T, glm::mat3>) {
                applyUniform(uniformName, UniformType::MAT3F, &val[0][0]);
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                applyUniform(uniformName, UniformType::MAT4F, &val[0][0]);
            } else {
                //static_assert(false, "Unknown uniform type!");
                logging::log(LEVEL_WARNING, "Bad uniform type for uniform \"{}\"!", uniformName);
            }
        }

        void bind ();
    };

    struct ShaderProgramSpec {
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, UniformType> uniforms;
    };


    class ShaderProgramBuilder {
    private:
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, UniformType> uniforms;
    public:
        ShaderProgramBuilder (const std::string& vertexPath, const std::string& fragmentPath) {
            shaderPaths[ShaderType::VERTEX] = vertexPath;
            shaderPaths[ShaderType::FRAGMENT] = fragmentPath;
        }

        ShaderProgramBuilder& addShader (ShaderType shaderType, const std::string& shaderPath) {
            if (shaderPaths.contains(shaderType)) {
                logging::log(LEVEL_ERROR, "Attempted to add shader \"{}\" to program but there already was a shader of that type!", shaderPath);
            } else {
                shaderPaths[shaderType] = shaderPath;
            }
            return *this;
        }

        template <typename T>
        ShaderProgramBuilder& addUniform (const std::string& uniform) {
            if constexpr (std::is_same_v<T, float>) {
                uniforms[uniform] = UniformType::FLOAT;
            } else if constexpr (std::is_same_v<T, int>) {
                uniforms[uniform] = UniformType::INT;
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                uniforms[uniform] = UniformType::VEC2F;
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                uniforms[uniform] = UniformType::VEC3F;
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                uniforms[uniform] = UniformType::VEC4F;
            } else if constexpr (std::is_same_v<T, glm::mat2>) {
                uniforms[uniform] = UniformType::MAT2F;
            } else if constexpr (std::is_same_v<T, glm::mat3>) {
                uniforms[uniform] = UniformType::MAT3F;
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                uniforms[uniform] = UniformType::MAT4F;
            }
            return *this;
        }

        ShaderProgramSpec build () {
            return {std::move(shaderPaths), std::move(uniforms)};
        }
    };
}