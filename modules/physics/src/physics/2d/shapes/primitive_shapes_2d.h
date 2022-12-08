#include "shape_2d.h"

namespace physics {
    class BoxShape2D : public CollisionShape2D {
    private:
    public:
        BoxShape2D (ShapeId id, ColliderId collider, ShapeId parent = {});

        util::Optional<CollisionResponse2D> collisionDetectionAccept(CollisionShape2D &other, glm::vec2 displacement) override;

        util::Optional<CollisionResponse2D> collisionDetectionVisit(BoxShape2D &other, glm::vec2 displacement) override;
        util::Optional<CollisionResponse2D> collisionDetectionVisit(CircleShape2D &other, glm::vec2 displacement) override;
    };

    class CircleShape2D : public CollisionShape2D {
    private:
        float localRadius;
        float worldRadius;
    public:
        CircleShape2D (float radius, ShapeId id, ColliderId collider, ShapeId parent = {});

        util::Optional<CollisionResponse2D> collisionDetectionAccept(CollisionShape2D &other, glm::vec2 displacement) override;

        util::Optional<CollisionResponse2D> collisionDetectionVisit(BoxShape2D &other, glm::vec2 displacement) override;
        util::Optional<CollisionResponse2D> collisionDetectionVisit(CircleShape2D &other, glm::vec2 displacement) override;

        void setRadius (float radius);
    };
}