#pragma once

#include "core/serialization/serializer_forward.h"
#include "physics/components/2D/collider.h"

namespace phenyl::physics {
class SATResult2D;
class Face2D;

class BoxCollider2D : public Collider2D {
public:
    std::optional<SATResult2D> collide (const BoxCollider2D& other);
    Face2D getSignificantFace (glm::vec2 normal);
    void applyFrameTransform (glm::mat2 transform);

    [[nodiscard]] glm::vec2 scale () const {
        return m_scale;
    }

    void setScale (glm::vec2 newScale) {
        m_scale = newScale;
    }

private:
    glm::vec2 m_scale;
    glm::mat2 m_frameTransform;

    friend class Physics2D;
    PHENYL_SERIALIZABLE_INTRUSIVE(BoxCollider2D)
};

PHENYL_DECLARE_SERIALIZABLE(BoxCollider2D)
} // namespace phenyl::physics
