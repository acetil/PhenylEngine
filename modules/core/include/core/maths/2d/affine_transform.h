#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::core {
class AffineTransform2D {
private:
    glm::mat3 m_mat = glm::identity<glm::mat3>();

public:
    AffineTransform2D () = default;

    AffineTransform2D (glm::mat3 mat) : m_mat{mat} {}

    explicit operator glm::mat3 () const noexcept {
        return m_mat;
    }

    glm::vec2 operator* (const glm::vec2& rhs) const noexcept {
        auto v = m_mat * glm::vec3{rhs, 1.0f};
        return {v.x, v.y};
    }

    AffineTransform2D operator* (const AffineTransform2D& rhs) const noexcept {
        return AffineTransform2D{m_mat * rhs.m_mat};
    }

    [[nodiscard]] glm::mat2 linearTransform () const noexcept {
        return glm::mat2{{m_mat[0][0], m_mat[0][1]}, {m_mat[1][0], m_mat[1][1]}};
    }

    [[nodiscard]] glm::mat2 rotation () const noexcept {
        auto transform = linearTransform();
        auto xLen = glm::length(transform[0]);
        auto yLen = glm::length(transform[1]);
        return {transform[0] / xLen, transform[1] / yLen};
    }
};
} // namespace phenyl::core
