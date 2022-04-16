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

    enum class ShaderDataType {
        FLOAT,
        INT,
        VEC2F,
        VEC3F,
        VEC4F,
        MAT2F,
        MAT3F,
        MAT4F,
        UNKNOWN
    };

    template <typename T>
    consteval ShaderDataType getShaderDataType () {
        if constexpr (std::is_same_v<T, float>) {
            return ShaderDataType::FLOAT;
        } else if constexpr (std::is_same_v<T, int>) {
            return ShaderDataType::INT;
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return ShaderDataType::VEC2F;
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return ShaderDataType::VEC3F;
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return ShaderDataType::VEC4F;
        } else if constexpr (std::is_same_v<T, glm::mat2>) {
            return ShaderDataType::MAT2F;
        } else if constexpr (std::is_same_v<T, glm::mat3>) {
            return ShaderDataType::MAT3F;
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            return ShaderDataType::MAT4F;
        } else {
            return ShaderDataType::UNKNOWN;
        }
    }

    const char* getUniformTypeName (ShaderDataType type);

    class RendererShaderProgram;

    class ShaderProgramNew {
    private:
        std::shared_ptr<RendererShaderProgram> internal;
        void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const unsigned char* dataPtr);
        template <typename T>
        void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const T* dataPtr) {
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
            constexpr auto uniformType = getShaderDataType<T>();

            /*if (uniformType == UniformType::UNKNOWN) {
                logging::log(LEVEL_WARNING, "Bad uniform type for uniform \"{}\"!", uniformName);
                return;
            }*/

            if constexpr (uniformType == ShaderDataType::FLOAT || uniformType == ShaderDataType::INT) {
                applyUniform(uniformName, uniformType, &val);
            } else if constexpr (uniformType == ShaderDataType::VEC2F || uniformType == ShaderDataType::VEC3F || uniformType == ShaderDataType::VEC4F) {
                applyUniform(uniformName, uniformType, &val[0]);
            } else if constexpr (uniformType == ShaderDataType::MAT2F || uniformType == ShaderDataType::MAT3F || uniformType == ShaderDataType::MAT4F) {
                applyUniform(uniformName, uniformType, &val[0][0]);
            } else {
                logging::log(LEVEL_WARNING, "Bad uniform type for uniform \"{}\"!", uniformName);
            }

            /*switch (uniformType) {
                case ShaderDataType::FLOAT:
                case ShaderDataType::INT:
                    applyUniform(uniformName, uniformType, &val);
                    break;
                case ShaderDataType::VEC2F:
                case ShaderDataType::VEC3F:
                case ShaderDataType::VEC4F:
                    applyUniform(uniformName, uniformType, &val[0]);
                    break;
                case ShaderDataType::MAT2F:
                case ShaderDataType::MAT3F:
                case ShaderDataType::MAT4F:
                    applyUniform(uniformName, uniformType, &val[0][0]);
                    break;
                default:
                    logging::log(LEVEL_WARNING, "Bad uniform type for uniform \"{}\"!", uniformName);
                    break;
            }*/

            /*if constexpr (std::is_same_v<T, float>) {
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
            }*/
        }

        void bind ();
    };

    struct ShaderProgramSpec {
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, ShaderDataType> uniforms;
    };


    class ShaderProgramBuilder {
    private:
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, ShaderDataType> uniforms;
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
            /*if constexpr (std::is_same_v<T, float>) {
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
            }*/
            uniforms[uniform] = getShaderDataType<T>();
            return *this;
        }

        ShaderProgramSpec build () {
            return {std::move(shaderPaths), std::move(uniforms)};
        }
    };
}