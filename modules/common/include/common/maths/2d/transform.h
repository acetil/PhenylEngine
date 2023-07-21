#pragma once

#include "graphics/maths_headers.h"

namespace common {
    class Transform2D {
    private:
        glm::vec2 positionVec{0, 0};
        glm::vec2 scaleVec{1.0f, 1.0f};
        glm::vec2 complexRotation{1.0f, 0.0f};

        Transform2D (glm::vec2 posVec, glm::vec2 scaleVec, glm::vec2 rotVec);

        friend bool phenyl_from_data (const util::DataValue& val, Transform2D& transform);
        friend util::DataValue phenyl_to_data (const Transform2D& transform);
    public:
        Transform2D () = default;

        [[nodiscard]] glm::vec2 position () const;
        Transform2D& translate (glm::vec2 deltaPos);
        [[nodiscard]] Transform2D withTranslation (glm::vec2 deltaPos) const;
        Transform2D& setPosition (glm::vec2 newPosition);
        [[nodiscard]] Transform2D withPosition (glm::vec2 newPosition) const;

        [[nodiscard]] glm::vec2 scale () const;
        Transform2D& scaleBy (glm::vec2 deltaScale);
        [[nodiscard]] Transform2D withScaleBy (glm::vec2 deltaScale) const;
        Transform2D& setScale (glm::vec2 newScale);
        [[nodiscard]] Transform2D withScale (glm::vec2 newScale) const;

        [[nodiscard]] float rotationAngle () const;
        Transform2D& rotateBy (float deltaAngle);
        [[nodiscard]] Transform2D withRotateBy (float deltaAngle) const;
        Transform2D& setRotation (float angle);
        Transform2D withRotation (float angle);

        [[nodiscard]] glm::mat2 scaleMatrix () const;
        [[nodiscard]] glm::mat2 rotMatrix () const;

        [[nodiscard]] glm::mat2 getMatrix () const;
    };

    bool phenyl_from_data (const util::DataValue& val, Transform2D& transform);
    util::DataValue phenyl_to_data (const Transform2D& transform);
}