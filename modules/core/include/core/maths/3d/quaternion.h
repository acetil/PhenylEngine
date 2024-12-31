#pragma once

#include "core/serialization/serializer_forward.h"
#include "logging/logging.h"

#include "graphics/maths_headers.h"

namespace phenyl::core {
    class Quaternion {
    private:
        glm::quat impl{1, 0, 0, 0};

        Quaternion (float w, float x, float y, float z) : impl{w, x, y, z} {}
        explicit Quaternion (glm::quat impl) : impl{impl} {}
        PHENYL_SERIALIZABLE_INTRUSIVE(Quaternion)
    public:
        static constexpr glm::vec3 ForwardVector{0, 0, 1};
        static constexpr glm::vec3 UpVector{0, 1, 0};

        Quaternion () = default;
        explicit Quaternion (float w, glm::vec3 vec) : impl{w, vec} {}

        static Quaternion Rotation (glm::vec3 axis, float theta) {
            PHENYL_DASSERT_MSG(glm::dot(axis, axis) == 1.0f, "Rotation axes must be normalised!");

            return Quaternion{glm::cos(theta / 2), glm::sin(theta / 2) * axis};
        }

        static Quaternion LookAt (glm::vec3 forward, glm::vec3 upAxis = UpVector) {
            PHENYL_DASSERT_MSG(glm::dot(upAxis, upAxis) == 1.0f, "Up axis must be normalised!");
            PHENYL_DASSERT_MSG((forward != glm::vec3{0, 0, 0}), "Forward axis cannot be zero vector!");

            // https://stackoverflow.com/questions/52413464/look-at-quaternion-using-up-vector/52551983
            forward = glm::normalize(forward); // Z
            auto sideAxis = glm::normalize(glm::cross(forward, upAxis)); // X
            auto rotatedUp = glm::cross(forward, sideAxis); // Y

            auto trace = sideAxis.x + rotatedUp.y + forward.z;
            if (trace > 0.0f) {
                auto s = 0.5f / glm::sqrt(trace + 1.0f);
                return Quaternion{0.25f / s, glm::vec3{rotatedUp.z - forward.y, forward.x - sideAxis.z, sideAxis.y - upAxis.x} * s};
            } else if (sideAxis.x > upAxis.y && sideAxis.x > forward.z) {
                auto s = 2.0f * glm::sqrt(1.0f + sideAxis.x - upAxis.y - forward.z);
                return Quaternion{(upAxis.z - forward.y) / s, 0.25f * s, (upAxis.x + sideAxis.y) / s, (forward.x + sideAxis.z) / s};
            } else if (upAxis.y > forward.z) {
                auto s = 2.0f * glm::sqrt(1.0f + upAxis.y - sideAxis.x - forward.z);
                return Quaternion{(forward.x - sideAxis.z) / s, (upAxis.x + sideAxis.y) / s, 0.25f * s, (forward.y + upAxis.z) / s};
            } else {
                auto s = 2.0f * glm::sqrt(1.0f + forward.z - sideAxis.x - upAxis.y);
                return Quaternion{(sideAxis.y - upAxis.x) / s, (forward.x + sideAxis.z) / s, (forward.y + upAxis.z) / s, 0.25f * s};
            }
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