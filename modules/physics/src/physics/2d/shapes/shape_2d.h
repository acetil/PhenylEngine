#pragma once

#include <memory>

#include "graphics/maths_headers.h"
#include "physics/physics.h"
#include "physics/2d/collision_response_2d.h"

namespace physics {
    class ShapeData2D;

    class CollisionShape2DBase {
    protected:
        ShapeId id;
        ColliderId collider;
        ShapeId parent;

        glm::mat2 localTransform;
        glm::vec2 localDisp;

        glm::mat2 worldTransform;
        glm::vec2 worldDisp;

        bool canRotate = false;
        bool canDistort = false;
        bool dirty = false;

        glm::vec2 worldRelLowBound{};
        glm::vec2 worldRelHighBound{};
        void markDirty ();
    public:
        CollisionShape2DBase (ShapeId id, ColliderId collider, ShapeId parent, bool canRotate, bool canDistort);
        virtual ~CollisionShape2DBase();

        ShapeId getId ();
        ColliderId getCollider ();
        ShapeId getParent ();

        bool isDirty ();

        //glm::mat2 getTransform ();
        void setTransform (glm::mat2 transform);

        //glm::vec2 getDisplacement ();
        void setDisplacement (glm::vec2 localDisplacement);

        void updateTransforms (glm::mat2 worldTransform, glm::vec2 worldDisplacement);


        virtual std::pair<glm::vec2, glm::vec2> updateGeometry () = 0;
    };

    class BoxShape2D;
    class CircleShape2D;

    class CollisionShape2D : public CollisionShape2DBase {
    private:
    public:
        CollisionShape2D (ShapeId id, ColliderId collider, ShapeId parent, bool canRotate, bool canDistort);
        virtual util::Optional<CollisionResponse2D> collisionDetectionAccept (CollisionShape2D& other, glm::vec2 displacement) = 0;

        virtual util::Optional<CollisionResponse2D> collisionDetectionVisit (BoxShape2D& other, glm::vec2 displacement) = 0;
        virtual util::Optional<CollisionResponse2D> collisionDetectionVisit (CircleShape2D& other, glm::vec2 displacement) = 0;
    };


    struct Shape2D {
        float outerRadius;

        std::uint64_t layers;
        std::uint64_t mask;

        Shape2D (float outerRadius, std::uint64_t layers, std::uint64_t mask) : outerRadius{outerRadius}, layers{layers}, mask{mask} {}

        bool shouldCollide (const Shape2D& other, glm::vec2 displacement) const;
    };

    struct BoxShape2D;

    using ShapeVariant2D = std::variant<BoxShape2D>;

    CollisionResponse2D collideShapes2D (const ShapeVariant2D& shape1, const ShapeVariant2D& shape2);
}