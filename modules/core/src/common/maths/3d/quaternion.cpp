#include "core/maths/3d/quaternion.h"

#include "core/detail/loggers.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"QUATERNION", detail::COMMON_LOGGER};

namespace {
    bool EpsilonEqual (glm::vec3 a, glm::vec3 b, float epsilon) {
        return glm::epsilonEqual(a[0], b[0], epsilon) && glm::epsilonEqual(a[1], b[1], epsilon) && glm::epsilonEqual(a[2], b[2], epsilon);
    }

    bool EpsilonEqual (glm::mat3 a, glm::mat3 b, float epsilon) {
        return EpsilonEqual(a[0], b[0], epsilon) && EpsilonEqual(a[1], b[1], epsilon) && EpsilonEqual(a[2], b[2], epsilon);
    }
}

Quaternion::Quaternion (glm::mat3 mat) {
    // https://math.stackexchange.com/questions/893984/conversion-of-rotation-matrix-to-quaternion
    // Note the glm::quat implementation requires w first
    //PHENYL_DASSERT_MSG((mat * glm::transpose(mat)) == glm::identity<glm::mat3>(), "Cannot convert non-orthonormal matrix {} to quaternion!", mat);
    PHENYL_DASSERT_MSG(EpsilonEqual((mat * glm::transpose(mat)), glm::identity<glm::mat3>(), 0.00001f), "Cannot convert non-orthonormal matrix {} to quaternion!", mat);
    PHENYL_DASSERT_MSG(glm::epsilonEqual(glm::determinant(mat), 1.0f, 0.00001f), "Cannot convert non-special orthonormal matrix {} to quaternion (determinant: {})!", mat, glm::determinant(mat));

    float t;
    if (mat[2][2] < 0.0f) {
        if (mat[0][0] > mat[1][1]) {
            t = 1.0f + mat[0][0] - mat[1][1] - mat[2][2];
            impl = {mat[1][2] - mat[2][1], t, mat[0][1] + mat[1][0], mat[2][0] + mat[0][2]};
        } else {
            t = 1.0f - mat[0][0] + mat[1][1] - mat[2][2];
            impl = {mat[2][0] - mat[0][2], mat[0][1] + mat[1][0], t, mat[1][2] + mat[2][1]};
        }
    } else {
        if (mat[0][0] < -mat[1][1]) {
            t = 1.0f - mat[0][0] - mat[1][1] + mat[2][2];
            impl = {mat[0][1] - mat[1][0], mat[2][0] + mat[0][2], mat[1][2] + mat[2][1], t};
        } else {
            t = 1.0f + mat[0][0] + mat[1][1] + mat[2][2];
            impl = {t, mat[1][2] - mat[2][1], mat[2][0] - mat[0][2], mat[0][1] - mat[1][0]};
        }
    }

    impl *= 0.5f / sqrtf(t);

    PHENYL_DASSERT(normalized());
}

Quaternion Quaternion::Rotation (glm::vec3 axis, float theta) {
    PHENYL_DASSERT_MSG(glm::epsilonEqual(glm::dot(axis, axis), 1.0f, 0.000001f), "Rotation axes must be normalised!");

    return Quaternion{glm::cos(theta / 2), glm::sin(theta / 2) * axis}.normalize();
}

Quaternion Quaternion::LookAtOld (glm::vec3 forward, glm::vec3 upAxis) {
    PHENYL_DASSERT_MSG(glm::epsilonEqual(glm::dot(upAxis, upAxis), 1.0f, 0.000001f), "Up axis must be normalised!");
    PHENYL_DASSERT_MSG((forward != glm::vec3{0, 0, 0}), "Forward axis cannot be zero vector!");
    PHENYL_DASSERT_MSG((glm::abs(glm::dot(glm::normalize(forward), upAxis)) != 1.0f), "Up axis cannot be parallel to forward axis");

    // https://stackoverflow.com/questions/52413464/look-at-quaternion-using-up-vector/52551983
    forward = glm::normalize(forward); // Z
    auto sideAxis = glm::normalize(glm::cross(upAxis, forward)); // X
    auto rotatedUp = glm::cross(forward, sideAxis); // Y
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(forward, rotatedUp), 0.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(forward, sideAxis), 0.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(sideAxis, rotatedUp), 0.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(rotatedUp, rotatedUp), 1.0f, 0.00001f));
    //PHENYL_DASSERT(glm::dot(sideAxis, rotatedUp) == 0.0f);

    auto trace = sideAxis.x + rotatedUp.y + forward.z;
    if (trace > 0.0f) {
        auto s = 0.5f / glm::sqrt(trace + 1.0f);
        return Quaternion{0.25f / s, glm::vec3{rotatedUp.z - forward.y, forward.x - sideAxis.z, sideAxis.y - rotatedUp.x} * s};
    } else if (sideAxis.x > rotatedUp.y && sideAxis.x > forward.z) {
        auto s = 2.0f * glm::sqrt(1.0f + sideAxis.x - rotatedUp.y - forward.z);
        return Quaternion{(rotatedUp.z - forward.y) / s, 0.25f * s, (rotatedUp.x + sideAxis.y) / s, (forward.x + sideAxis.z) / s};
    } else if (rotatedUp.y > forward.z) {
        auto s = 2.0f * glm::sqrt(1.0f + rotatedUp.y - sideAxis.x - forward.z);
        return Quaternion{(forward.x - sideAxis.z) / s, (rotatedUp.x + sideAxis.y) / s, 0.25f * s, (forward.y + rotatedUp.z) / s};
    } else {
        auto s = 2.0f * glm::sqrt(1.0f + forward.z - sideAxis.x - rotatedUp.y);
        return Quaternion{(sideAxis.y - rotatedUp.x) / s, (forward.x + sideAxis.z) / s, (forward.y + rotatedUp.z) / s, 0.25f * s};
    }
}

Quaternion Quaternion::LookAt (glm::vec3 forward, glm::vec3 upAxis) {
    //return LookAtOld(forward, upAxis);
    PHENYL_DASSERT_MSG(glm::epsilonEqual(glm::dot(upAxis, upAxis), 1.0f, 0.000001f), "Up axis must be normalised!");
    PHENYL_DASSERT_MSG((forward != glm::vec3{0, 0, 0}), "Forward axis cannot be zero vector!");
    PHENYL_DASSERT_MSG((glm::abs(glm::dot(glm::normalize(forward), upAxis)) != 1.0f), "Up axis cannot be parallel to forward axis");

    // https://stackoverflow.com/questions/52413464/look-at-quaternion-using-up-vector/52551983
    forward = glm::normalize(forward); // Z
    auto sideAxis = glm::normalize(glm::cross(forward, upAxis)); // X
    auto rotatedUp = glm::cross(forward, sideAxis); // Y

    PHENYL_TRACE(LOGGER, "LookAt X={}", sideAxis);
    PHENYL_TRACE(LOGGER, "LookAt Y={}", rotatedUp);
    PHENYL_TRACE(LOGGER, "LookAt Z={}", forward);


    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(sideAxis, sideAxis), 1.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(rotatedUp, rotatedUp), 1.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(forward, forward), 1.0f, 0.00001f));

    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(forward, rotatedUp), 0.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(forward, sideAxis), 0.0f, 0.00001f));
    PHENYL_DASSERT(glm::epsilonEqual(glm::dot(sideAxis, rotatedUp), 0.0f, 0.00001f));

    return Quaternion{glm::mat3{sideAxis, rotatedUp, forward}};
}

glm::vec3 Quaternion::axis () const noexcept {
    return xyz() / std::sin(eulerAngle() / 2.0f);
}

float Quaternion::eulerAngle () const noexcept {
    return std::acosf(w()) * 2.0f;
}

Quaternion Quaternion::conjugate () const noexcept {
    //return Quaternion{glm::conjugate(impl)};
    return Quaternion{w(), -xyz()};
}

Quaternion Quaternion::inverse () const noexcept {
    PHENYL_DASSERT(normalized());
    return conjugate();
}

Quaternion Quaternion::normalize () const noexcept {
    return Quaternion{impl / length()};
}

bool Quaternion::normalized () const noexcept {
    return glm::epsilonEqual(lengthSquared(), 1.0f, 0.0001f);
}

float Quaternion::length () const noexcept {
    return glm::sqrt(lengthSquared());
}

float Quaternion::lengthSquared () const noexcept {
    return ((*this) * (conjugate())).w();
}

glm::vec3 Quaternion::operator* (glm::vec3 vec) const noexcept {
    //return (inverse() * (Quaternion{0, vec} * (*this))).xyz();
    return (*this * Quaternion(0, vec) * inverse()).xyz();
}
