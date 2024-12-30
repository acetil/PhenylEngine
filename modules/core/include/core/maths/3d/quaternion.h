#pragma once

#include "core/serialization/serializer_forward.h"
#include "logging/logging.h"

#include "graphics/maths_headers.h"

namespace phenyl::core {
    class Quaternion {
    private:
        glm::quat impl{1, 0, 0, 0};

        explicit Quaternion (glm::quat impl) : impl{impl} {}
        PHENYL_SERIALIZABLE_INTRUSIVE(Quaternion)
    public:
        Quaternion () = default;
        explicit Quaternion (float w, glm::vec3 vec) : impl{w, vec} {}

        static Quaternion Rotation (glm::vec3 axis, float theta) {
            PHENYL_DASSERT_MSG(glm::dot(axis, axis) == 1.0f, "Rotation axes must be normalised!");

            return Quaternion{glm::cos(theta / 2), glm::sin(theta / 2) * axis};
        }

        float w () const noexcept {
            return impl.w;
        }

        glm::vec3 xyz () const noexcept {
            return glm::vec3{impl.x, impl.y, impl.z};
        }

        Quaternion conjugate () const noexcept {
            return Quaternion{glm::conjugate(impl)};
        }

        Quaternion inverse () const noexcept {
            return conjugate() / lengthSquared();
        }

        Quaternion normalize () const noexcept {
            return Quaternion{impl / length()};
        }

        float length () const noexcept {
            return glm::sqrt(lengthSquared());
        }

        float lengthSquared () const noexcept {
            return ((*this) * (conjugate())).w();
        }

        Quaternion operator+ (const Quaternion& other) const noexcept {
            return Quaternion{impl + other.impl};
        }

        Quaternion& operator+= (const Quaternion& other) noexcept {
            impl += other.impl;
            return *this;
        }

        Quaternion operator- (const Quaternion& other) const noexcept {
            return Quaternion{impl - other.impl};
        }

        Quaternion& operator-= (const Quaternion& other) noexcept {
            impl += other.impl;
            return *this;
        }

        Quaternion operator* (const Quaternion& other) const noexcept {
            return Quaternion{impl * other.impl};
        }

        Quaternion& operator*= (const Quaternion& other) noexcept {
            impl *= other.impl;
            return *this;
        }

        Quaternion operator* (float f) const noexcept {
            return Quaternion{impl * f};
        }

        Quaternion& operator*= (float f) noexcept {
            impl *= f;
            return *this;
        }

        Quaternion operator/ (float f) const noexcept {
            return Quaternion{impl / f};
        }

        Quaternion& operator/= (float f) noexcept {
            impl /= f;
            return *this;
        }

        glm::vec3 operator* (glm::vec3 vec) const noexcept {
            return (inverse() * Quaternion{0, vec} * (*this)).xyz();
        }

        explicit operator glm::mat3 () const noexcept {
            return glm::mat3_cast(impl);
        }

        explicit operator glm::mat4 () const noexcept {
            return glm::mat4_cast(impl);
        }
    };
}