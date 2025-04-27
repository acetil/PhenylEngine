#pragma once

#include "core/serialization/serializer_impl.h"
#include "graphics/maths_headers.h"

namespace phenyl::core {
    class Transform2D {
    private:
        glm::vec2 m_position{0, 0};
        glm::vec2 m_scale{1.0f, 1.0f};
        glm::vec2 m_complexRotation{1.0f, 0.0f};

        Transform2D (glm::vec2 posVec, glm::vec2 scaleVec, glm::vec2 rotVec);

        PHENYL_SERIALIZABLE_INTRUSIVE(Transform2D)
    public:
        Transform2D () = default;

        [[nodiscard]] inline glm::vec2 position () const {
            return m_position;
        }
        Transform2D& translate (glm::vec2 deltaPos);
        [[nodiscard]] Transform2D withTranslation (glm::vec2 deltaPos) const;
        Transform2D& setPosition (glm::vec2 newPosition);
        [[nodiscard]] Transform2D withPosition (glm::vec2 newPosition) const;

        [[nodiscard]] inline glm::vec2 scale () const {
            return m_scale;
        }
        Transform2D& scaleBy (glm::vec2 deltaScale);
        [[nodiscard]] Transform2D withScaleBy (glm::vec2 deltaScale) const;
        Transform2D& setScale (glm::vec2 newScale);
        [[nodiscard]] Transform2D withScale (glm::vec2 newScale) const;

        [[nodiscard]] float rotationAngle () const;
        Transform2D& rotateBy (float deltaAngle);
        [[nodiscard]] Transform2D withRotateBy (float deltaAngle) const;
        Transform2D& setRotation (float angle);
        Transform2D withRotation (float angle);

        [[nodiscard]] inline glm::mat2 scaleMatrix () const {
            return {{m_scale.x, 0}, {0, m_scale.y}};
        }
        [[nodiscard]] inline glm::mat2 rotMatrix () const {
            return {{m_complexRotation.x, m_complexRotation.y}, {-m_complexRotation.y, m_complexRotation.x}};
        }

        [[nodiscard]] inline glm::mat2 getMatrix () const {
            return scaleMatrix() * rotMatrix();
        }

        [[nodiscard]] inline glm::vec2 apply (glm::vec2 vec) const {
            return getMatrix() * vec + position();
        }
    };
}