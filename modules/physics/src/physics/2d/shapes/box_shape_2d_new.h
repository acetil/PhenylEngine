#pragma once

#include "shape_2d_new.h"
#include "util/optional.h"

namespace physics {
    class BoxShape2DNew : public Shape2DNew {
    private:
        glm::mat2 scaleMatrix;
        glm::mat2 frameTransform;
        glm::mat2 invFrameTransform;
    public:
        BoxShape2DNew (ColliderId collider, glm::vec2 scale, std::uint64_t layers, std::uint64_t mask);
        BoxShape2DNew (ColliderId collider, glm::mat2 scaleMat, std::uint64_t layers, std::uint64_t mask);

        glm::vec2 getScale () const; // TODO: remove?
        void setScale (glm::vec2 scale);

        void applyTransform (glm::mat2 transform) override;

        util::Optional<glm::vec2> collide (const BoxShape2DNew& other);

        util::DataValue serialise() const override;

        static util::Optional<BoxShape2DNew> deserialise (const util::DataObject& val, ColliderId collider, std::uint64_t layers, std::uint64_t mask);
    };
}