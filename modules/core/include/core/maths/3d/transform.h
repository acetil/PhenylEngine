#pragma once

#include "graphics/maths_headers.h"
#include "quaternion.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
    class Transform3D {
    private:
        glm::vec3 transformPos{0, 0, 0};
        glm::vec3 transformScale{1, 1, 1};
        Quaternion transformRot{};

        PHENYL_SERIALIZABLE_INTRUSIVE(Transform3D)
    public:
        explicit Transform3D (glm::vec3 pos = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f}, Quaternion rotation = {}) : transformPos{pos}, transformScale{scale}, transformRot{rotation} {}

        glm::vec3 position () const noexcept {
            return transformPos;
        }

        glm::vec3 scale () const noexcept {
            return transformScale;
        }

        Quaternion rotation () const noexcept {
            return transformRot;
        }

        Transform3D& translate (glm::vec3 delta) noexcept {
            transformPos += delta;
            return *this;
        }
        Transform3D withTranslation (glm::vec3 delta) const noexcept {
            return Transform3D{transformPos + delta, transformScale, transformRot};
        }

        Transform3D& setPosition (glm::vec3 newPos) noexcept {
            transformPos = newPos;
            return *this;
        }
        Transform3D withPosition (glm::vec3 newPos) const noexcept {
            return Transform3D{newPos, transformScale, transformRot};
        }

        Transform3D& scaleBy (glm::vec3 deltaScale) noexcept {
            transformScale *= deltaScale;
            return *this;
        }
        Transform3D withScaleBy (glm::vec3 deltaScale) const noexcept {
            return Transform3D{transformPos, transformScale * deltaScale, transformRot};
        }

        Transform3D& setScale (glm::vec3 scale) noexcept {
            transformScale = scale;
            return *this;
        }
        Transform3D withScale (glm::vec3 scale) const noexcept {
            return Transform3D{transformPos, scale, transformRot};
        }

        Transform3D& rotateBy (Quaternion q) noexcept {
            PHENYL_DASSERT_MSG(q.lengthSquared() == 1.0f, "Cannot rotate by non-normalized quaternion!");
            transformRot = q * transformRot;
            return *this;
        }
        Transform3D withRotateBy (Quaternion q) const noexcept {
            PHENYL_DASSERT_MSG(q.lengthSquared() == 1.0f, "Cannot rotate by non-normalized quaternion!");
            return Transform3D{transformPos, transformScale, q * transformRot};
        }

        Transform3D& setRotation (Quaternion q) noexcept {
            PHENYL_DASSERT_MSG(q.lengthSquared() == 1.0f, "Cannot rotate by non-normalized quaternion!");
            transformRot = q;
            return *this;
        }
        Transform3D withRotation (Quaternion q) const noexcept {
            PHENYL_DASSERT_MSG(q.lengthSquared() == 1.0f, "Cannot rotate by non-normalized quaternion!");
            return Transform3D{transformPos, transformScale, q};
        }

        glm::mat3 scaleMatrix () const noexcept {
            return {
                {transformScale.x, 0, 0},
                {0, transformScale.y, 0},
                {0, 0, transformScale.z}
            };
        }

        glm::mat4 transformMatrx () const noexcept {
            glm::mat4 translateScale = {
                {transformScale.x, 0, 0, transformPos.x},
                {0, transformScale.y, 0, transformPos.y},
                {0, 0, transformScale.z, transformPos.z},
                {0, 0, 0, 1}
            };

            return translateScale * static_cast<glm::mat4>(transformRot);
        }

        glm::vec3 apply (glm::vec3 v) const noexcept {
            return (rotation() * v) * scale() + position();
        }
    };
}