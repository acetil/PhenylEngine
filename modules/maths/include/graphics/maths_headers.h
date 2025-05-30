#pragma once

#include <format>

// TODO
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace phenyl::util {
class DataValue;

constexpr glm::vec2 SafeNormalize (const glm::vec2 inVec) {
    return inVec.x == 0.0f && inVec.y == 0.0f ? inVec : glm::normalize(inVec);
}
} // namespace phenyl::util

template <>
struct std::formatter<glm::vec2, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format (const glm::vec2& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}>", vec.x, vec.y);
    }
};

template <>
struct std::formatter<glm::vec3, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format string for vec3");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (const glm::vec3& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}, {}>", vec.x, vec.y, vec.z);
    }
};

template <>
struct std::formatter<glm::vec4, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format string for vec4");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (const glm::vec4& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}, {}, {}>", vec.x, vec.y, vec.z, vec.w);
    }
};

template <>
struct std::formatter<glm::mat3, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format string for mat3");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (const glm::mat3& mat, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}, {}>", mat[0], mat[1], mat[2]);
    }
};
