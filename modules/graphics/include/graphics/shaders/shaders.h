#pragma once

#include <memory>
#include <utility>
#include "logging/logging.h"
#include "graphics/detail/loggers.h"
#include "graphics/maths_headers.h"
#include "util/map.h"

namespace phenyl::graphics {

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

    class RendererShader;

    class Shader {
    private:
        std::shared_ptr<RendererShader> internal;
        void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const unsigned char* dataPtr);
        template <typename T>
        void applyUniform (const std::string& uniformName, ShaderDataType uniformType, const T* dataPtr) {
            applyUniform(uniformName, uniformType, (const unsigned char*)dataPtr);
        }
        /*inline bool debugCheckShared () {
#ifndef NDEBUG
            if (!internal) {
                logging::log(LEVEL_ERROR, "Empty ShaderProgram instance! Engine will crash here in release!");
                return false;
            }
#endif
            return true;
        }*/
    public:
        Shader () = default;
        explicit Shader (std::shared_ptr<RendererShader> _internal) : internal{std::move(_internal)} {};
        ~Shader();

        template <typename T>
        void applyUniform (const std::string& uniformName, const T& val) {
            constexpr auto uniformType = getShaderDataType<T>();

            if constexpr (uniformType == ShaderDataType::FLOAT || uniformType == ShaderDataType::INT) {
                applyUniform(uniformName, uniformType, &val);
            } else if constexpr (uniformType == ShaderDataType::VEC2F || uniformType == ShaderDataType::VEC3F || uniformType == ShaderDataType::VEC4F) {
                applyUniform(uniformName, uniformType, &val[0]);
            } else if constexpr (uniformType == ShaderDataType::MAT2F || uniformType == ShaderDataType::MAT3F || uniformType == ShaderDataType::MAT4F) {
                applyUniform(uniformName, uniformType, &val[0][0]);
            } else {
                PHENYL_LOGW(detail::SHADER_LOGGER, "Bad uniform type for uniform \"{}\"!", uniformName);
            }
        }

        void bind ();
    };

    struct ShaderSpec {
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, ShaderDataType> uniforms;
    };

    struct ShaderSourceSpec {
        util::Map<ShaderType, std::string> shaderSources;
        util::Map<std::string, ShaderDataType> uniforms;
    };


    class ShaderBuilder {
    private:
        util::Map<ShaderType, std::string> shaderPaths;
        util::Map<std::string, ShaderDataType> uniforms;
    public:
        ShaderBuilder (const std::string& vertexPath, const std::string& fragmentPath) {
            shaderPaths[ShaderType::VERTEX] = vertexPath;
            shaderPaths[ShaderType::FRAGMENT] = fragmentPath;
        }

        ShaderBuilder& addShader (ShaderType shaderType, const std::string& shaderPath) {
            if (shaderPaths.contains(shaderType)) {
                PHENYL_LOGE(detail::SHADER_LOGGER, "Attempted to insert shader \"{}\" to program but there already was a shader of that type!", shaderPath);
            } else {
                shaderPaths[shaderType] = shaderPath;
            }
            return *this;
        }

        template <typename T>
        ShaderBuilder& addUniform (const std::string& uniform) {
            uniforms[uniform] = getShaderDataType<T>();
            return *this;
        }

        ShaderSpec build () {
            return {std::move(shaderPaths), std::move(uniforms)};
        }
    };

    class ShaderSourceBuilder {
    private:
        util::Map<ShaderType, std::string> shaderSources;
        util::Map<std::string, ShaderDataType> uniforms;
    public:
        ShaderSourceBuilder (const std::string& vertexSource, const std::string& fragmentSource) {
            shaderSources[ShaderType::VERTEX] = vertexSource;
            shaderSources[ShaderType::FRAGMENT] = fragmentSource;
        }

        template <typename T>
        ShaderSourceBuilder& addUniform (const std::string& uniform) {
            uniforms[uniform] = getShaderDataType<T>();
            return *this;
        }

        ShaderSourceSpec build () {
            return {std::move(shaderSources), std::move(uniforms)};
        }
    };
}