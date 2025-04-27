#pragma once

#include "graphics/maths_headers.h"
#include "quaternion.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
    class Transform3D {
    private:
        glm::vec3 m_position{0, 0, 0};
        glm::vec3 m_scale{1, 1, 1};
        Quaternion m_rotation{};

        PHENYL_SERIALIZABLE_INTRUSIVE(Transform3D)
    public:
        explicit Transform3D (glm::vec3 pos = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f}, Quaternion rotation = {}) : m_position{pos}, m_scale{scale}, m_rotation{rotation} {}

        glm::vec3 position () const noexcept {
            return m_position;
        }

        glm::vec3 scale () const noexcept {
            return m_scale;
        }

        Quaternion rotation () const noexcept {
            return m_rotation;
        }

        Transform3D& translate (glm::vec3 delta) noexcept {
            m_position += delta;
            return *this;
        }
        Transform3D withTranslation (glm::vec3 delta) const noexcept {
            return Transform3D{m_position + delta, m_scale, m_rotation};
        }

        Transform3D& setPosition (glm::vec3 newPos) noexcept {
            m_position = newPos;
            return *this;
        }
        Transform3D withPosition (glm::vec3 newPos) const noexcept {
            return Transform3D{newPos, m_scale, m_rotation};
        }

        Transform3D& scaleBy (glm::vec3 deltaScale) noexcept {
            m_scale *= deltaScale;
            return *this;
        }
        Transform3D withScaleBy (glm::vec3 deltaScale) const noexcept {
            return Transform3D{m_position, m_scale * deltaScale, m_rotation};
        }

        Transform3D& setScale (glm::vec3 scale) noexcept {
            m_scale = scale;
            return *this;
        }
        Transform3D withScale (glm::vec3 scale) const noexcept {
            return Transform3D{m_position, scale, m_rotation};
        }

        Transform3D& rotateBy (Quaternion q) noexcept {
            PHENYL_DASSERT_MSG(q.normalized(), "Cannot rotate by non-normalized quaternion!");
            m_rotation = q * m_rotation;
            return *this;
        }
        Transform3D withRotateBy (Quaternion q) const noexcept {
            PHENYL_DASSERT_MSG(q.normalized(), "Cannot rotate by non-normalized quaternion!");
            return Transform3D{m_position, m_scale, q * m_rotation};
        }

        Transform3D& setRotation (Quaternion q) noexcept {
            PHENYL_DASSERT_MSG(q.normalized(), "Cannot rotate by non-normalized quaternion!");
            m_rotation = q;
            return *this;
        }
        Transform3D withRotation (Quaternion q) const noexcept {
            PHENYL_DASSERT_MSG(q.normalized(), "Cannot rotate by non-normalized quaternion!");
            return Transform3D{m_position, m_scale, q};
        }

        glm::mat4 scaleMatrix () const noexcept {
            return {
                {m_scale.x, 0, 0, 0},
                {0, m_scale.y, 0, 0},
                {0, 0, m_scale.z, 0},
                {0, 0, 0, 1}
            };
        }

        glm::mat4 transformMatrx () const noexcept {
            glm::mat4 translationMat = {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {m_position.x, m_position.y, m_position.z, 1}
            };

            return translationMat * static_cast<glm::mat4>(rotation()) * scaleMatrix();
        }

        glm::vec3 apply (glm::vec3 v) const noexcept {
            return rotation() * (v * scale()) + position();
        }
    };
}