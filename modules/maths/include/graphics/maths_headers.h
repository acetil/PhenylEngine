#pragma once

#include <format>

// TODO
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace phenyl::util {
    class DataValue;
}

namespace glm {
    bool phenyl_from_data (const phenyl::util::DataValue& val, vec2& v);
    phenyl::util::DataValue phenyl_to_data (const vec2& v);

    bool phenyl_from_data(const phenyl::util::DataValue& val, vec4& v);
    phenyl::util::DataValue phenyl_to_data (const vec4& v);

    bool phenyl_from_data (const phenyl::util::DataValue& val, glm::mat2& m);
    phenyl::util::DataValue phenyl_to_data (const glm::mat2& m);
}

template <>
struct std::formatter<glm::vec2, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format(const glm::vec2& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}>", vec.x, vec.y);
    }
};

template <>
struct std::formatter<glm::vec3, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end()) {
            throw std::format_error("Invalid format string for vec3");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(const glm::vec3& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}, {}>", vec.x, vec.y, vec.z);
    }
};

template <>
struct std::formatter<glm::vec4, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end()) {
            throw std::format_error("Invalid format string for vec4");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(const glm::vec4& vec, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "<{}, {}, {}, {}>", vec.x, vec.y, vec.z, vec.w);
    }
};