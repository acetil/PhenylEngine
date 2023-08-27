#pragma once

#include "shape_2d.h"
#include "collisions_2d.h"
#include "util/optional.h"

namespace physics {
    class BoxShape2D : public Shape2D {
    private:
        glm::mat2 scaleMatrix;
        glm::mat2 frameTransform;
        glm::mat2 invFrameTransform;
    public:
        BoxShape2D (ColliderId collider, glm::vec2 scale, std::uint64_t layers, std::uint64_t mask);
        BoxShape2D (ColliderId collider, glm::mat2 scaleMat, std::uint64_t layers, std::uint64_t mask);

        glm::vec2 getScale () const; // TODO: remove?
        void setScale (glm::vec2 scale);

        void applyTransform (glm::mat2 transform) override;
        glm::mat2 getTransform () const;

        util::Optional<SATResult2D> collide (const BoxShape2D& other);

        util::DataValue serialise() const override;

        static util::Optional<BoxShape2D> deserialise (const util::DataObject& val, ColliderId collider, std::uint64_t layers, std::uint64_t mask);
    };
}