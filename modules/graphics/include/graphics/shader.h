#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "graphics/maths_headers.h"
#include "logging/logging.h"

namespace phenyl::graphics {
    enum class ShaderSourceType {
        FRAGMENT,
        VERTEX
    };

    enum class ShaderDataType {
        FLOAT32,
        INT16,
        INT32,
        VEC2F,
        VEC3F,
        VEC4F,
        MAT2F,
        MAT3F,
        MAT4F,
        UNKNOWN
    };

    template <typename T>
    consteval ShaderDataType GetShaderDataType () {
        if constexpr (std::is_same_v<T, float>) {
            return ShaderDataType::FLOAT32;
        } else if constexpr (std::is_same_v<T, int16_t>) {
            return ShaderDataType::INT16;
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return ShaderDataType::INT32;
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

    class IShader {
    public:
        virtual ~IShader() = default;

        [[nodiscard]] virtual std::size_t hash () const noexcept = 0;
        [[nodiscard]] virtual std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept = 0;
        [[nodiscard]] virtual std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept = 0;
    };

    class Shader {
    private:
        std::unique_ptr<IShader> shader;
        std::size_t hash{0};
    public:
        Shader () = default;
        explicit Shader (std::unique_ptr<IShader> shader) : shader{std::move(shader)}, hash{this->shader->hash()} {}

        explicit operator bool () const noexcept {
            return (bool)shader;
        }

        [[nodiscard]] std::optional<unsigned int> uniformLocation (const std::string& uniform) const noexcept {
            return shader->getUniformLocation(uniform);
        }

        [[nodiscard]] std::optional<unsigned int> samplerLocation (const std::string& sampler) const noexcept {
            return shader->getSamplerLocation(sampler);
        }

        IShader& getUnderlying () {
            PHENYL_DASSERT(shader);
            return *shader;
        }
    };
}