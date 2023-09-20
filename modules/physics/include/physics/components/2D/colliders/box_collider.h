#include "physics/components/2D/collider.h"
#include "util/optional.h"
#include "common/serializer_intrusive.h"

#pragma once

namespace phenyl::physics {
    class SATResult2D;
    class Face2D;
    class BoxCollider2D : public Collider2D {
    private:
        glm::vec2 scale;
        glm::mat2 frameTransform;

        void applyFrameTransform (glm::mat2 transform);

        util::Optional<SATResult2D> collide (const BoxCollider2D& other);
        Face2D getSignificantFace (glm::vec2 normal);

        friend class PhysicsObject2D;

        PHENYL_SERIALIZE_INTRUSIVE(BoxCollider2D)
    public:
        [[nodiscard]] glm::vec2 getScale () const {
            return scale;
        }

        void setScale (glm::vec2 newScale) {
            scale = newScale;
        }
    };
}