#pragma once

#include "core/serialization/serializer_forward.h"
#include "graphics/maths_headers.h"
#include "logging/logging.h"

namespace phenyl::core {
class Quaternion {
public:
    static constexpr glm::vec3 ForwardVector{0, 0, 1};
    static constexpr glm::vec3 UpVector{0, 1, 0};

    Quaternion () = default;

    explicit Quaternion (glm::mat3 mat);

    explicit Quaternion (float w, glm::vec3 vec) : m_impl{w, vec} {}

    static Quaternion Rotation (glm::vec3 axis, float theta);
    static Quaternion LookAtOld (glm::vec3 forward, glm::vec3 upAxis = UpVector);
    static Quaternion LookAt (glm::vec3 forward, glm::vec3 upAxis = UpVector);

    float w () const noexcept {
        return m_impl.w;
    }

    glm::vec3 xyz () const noexcept {
        return glm::vec3{m_impl.x, m_impl.y, m_impl.z};
    }

    glm::vec3 axis () const noexcept;
    float eulerAngle () const noexcept;

    Quaternion conjugate () const noexcept;
    Quaternion inverse () const noexcept;
    Quaternion normalize () const noexcept;

    bool normalized () const noexcept;
    float length () const noexcept;
    float lengthSquared () const noexcept;

    Quaternion operator+ (const Quaternion& other) const noexcept {
        return Quaternion{m_impl + other.m_impl};
    }

    Quaternion& operator+= (const Quaternion& other) noexcept {
        m_impl += other.m_impl;
        return *this;
    }

    Quaternion operator- (const Quaternion& other) const noexcept {
        return Quaternion{m_impl - other.m_impl};
    }

    Quaternion& operator-= (const Quaternion& other) noexcept {
        m_impl += other.m_impl;
        return *this;
    }

    Quaternion operator* (const Quaternion& other) const noexcept {
        return Quaternion{m_impl * other.m_impl};
    }

    Quaternion& operator*= (const Quaternion& other) noexcept {
        m_impl *= other.m_impl;
        return *this;
    }

    Quaternion operator* (float f) const noexcept {
        return Quaternion{m_impl * f};
    }

    Quaternion& operator*= (float f) noexcept {
        m_impl *= f;
        return *this;
    }

    Quaternion operator/ (float f) const noexcept {
        return Quaternion{m_impl / f};
    }

    Quaternion& operator/= (float f) noexcept {
        m_impl /= f;
        return *this;
    }

    glm::vec3 operator* (glm::vec3 vec) const noexcept;

    explicit operator glm::mat3 () const noexcept {
        return glm::mat3_cast(m_impl);
    }

    explicit operator glm::mat4 () const noexcept {
        return glm::mat4_cast(m_impl);
    }

private:
    glm::quat m_impl{1, 0, 0, 0};

    Quaternion (float w, float x, float y, float z) : m_impl{w, x, y, z} {}

    explicit Quaternion (glm::quat impl) : m_impl{impl} {}
    PHENYL_SERIALIZABLE_INTRUSIVE(Quaternion)
};
} // namespace phenyl::core
