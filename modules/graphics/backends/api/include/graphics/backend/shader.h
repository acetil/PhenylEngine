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

    inline ShaderDataType ShaderTypeFromName (std::string_view type) {
        if (type == "float") {
            return ShaderDataType::FLOAT32;
        } else if (type == "int16") {
            return ShaderDataType::INT16;
        } else if (type == "int32") {
            return ShaderDataType::INT32;
        } else if (type == "vec2f") {
            return ShaderDataType::VEC2F;
        } else if (type == "vec3f") {
            return ShaderDataType::VEC3F;
        } else if (type == "vec4f") {
            return ShaderDataType::VEC4F;
        } else if (type == "mat2f") {
            return ShaderDataType::MAT2F;
        } else if (type == "mat3f") {
            return ShaderDataType::MAT3F;
        } else if (type == "mat4f") {
            return ShaderDataType::MAT4F;
        } else {
            return ShaderDataType::UNKNOWN;
        }
    }

    inline constexpr std::size_t ShaderTypeSize (ShaderDataType type) {
        switch (type) {
            case ShaderDataType::FLOAT32:
                return sizeof(float);
            case ShaderDataType::INT16:
                return sizeof(std::uint16_t);
            case ShaderDataType::INT32:
                return sizeof(std::uint32_t);
            case ShaderDataType::VEC2F:
                return sizeof(glm::vec2);
            case ShaderDataType::VEC3F:
                return sizeof(glm::vec3);
            case ShaderDataType::VEC4F:
                return sizeof(glm::vec4);
            case ShaderDataType::MAT2F:
                return sizeof(glm::mat2);
            case ShaderDataType::MAT3F:
                return sizeof(glm::mat3);
            case ShaderDataType::MAT4F:
                return sizeof(glm::mat4);
            default:
                break;
        }

        return 0;
    }

    class IShader {
    public:
        virtual ~IShader() = default;

        [[nodiscard]] virtual std::size_t hash () const noexcept = 0;
        [[nodiscard]] virtual std::optional<unsigned int> getAttribLocation (const std::string& attrib) const noexcept = 0;
        [[nodiscard]] virtual std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept = 0;
        [[nodiscard]] virtual std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept = 0;

        virtual std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock, const std::string& uniform) const noexcept = 0;
        virtual std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept = 0;
    };

    class Shader {
    public:
        Shader () = default;
        explicit Shader (std::unique_ptr<IShader> shader) : m_shader{std::move(shader)}, m_hash{this->m_shader ? this->m_shader->hash() : 0} {}

        explicit operator bool () const noexcept {
            return (bool)m_shader;
        }

        [[nodiscard]] std::optional<unsigned int> uniformLocation (const std::string& uniform) const noexcept {
            PHENYL_DASSERT(m_shader);
            return m_shader->getUniformLocation(uniform);
        }

        [[nodiscard]] std::optional<unsigned int> samplerLocation (const std::string& sampler) const noexcept {
            PHENYL_DASSERT(m_shader);
            return m_shader->getSamplerLocation(sampler);
        }

        [[nodiscard]] std::optional<unsigned int> attribLocation (const std::string& attrib) const noexcept {
            PHENYL_DASSERT(m_shader);
            return m_shader->getAttribLocation(attrib);
        }

        std::optional<std::size_t> uniformOffset (const std::string& uniformBlock, const std::string& uniform) const noexcept {
            PHENYL_DASSERT(m_shader);
            return m_shader->getUniformOffset(uniformBlock, uniform);
        }

        std::optional<std::size_t> uniformBlockSize (const std::string& uniformBlock) const noexcept {
            PHENYL_DASSERT(m_shader);
            return m_shader->getUniformBlockSize(uniformBlock);
        }

        IShader& getUnderlying () {
            PHENYL_DASSERT(m_shader);
            return *m_shader;
        }

    private:
        std::unique_ptr<IShader> m_shader;
        std::size_t m_hash{0};
    };
}

template<>
struct std::formatter<phenyl::graphics::ShaderSourceType, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format (const phenyl::graphics::ShaderSourceType& type, FmtContext& ctx) const {
        switch (type) {
            case phenyl::graphics::ShaderSourceType::FRAGMENT:
                return std::format_to(ctx.out(), "FRAGMENT");
            case phenyl::graphics::ShaderSourceType::VERTEX:
                return std::format_to(ctx.out(), "VERTEX");
            default:
                return std::format_to(ctx.out(), "UNKNOWN");
        }
    }
};