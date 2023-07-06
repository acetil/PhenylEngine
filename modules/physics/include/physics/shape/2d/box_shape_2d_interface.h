#pragma once

#include "physics/shape/physics_shape.h"
#include "physics/physics.h"
#include "graphics/maths_headers.h"

namespace physics {
    class BoxShape2DNew;

    template <>
    class ShapeView<BoxShape2DNew> {
    private:
        BoxShape2DNew& shape;

        ShapeView (void* shapeData) : shape{*(static_cast<BoxShape2DNew*>(shapeData))} {}

        friend class ShapeDataNew;

    public:
        glm::vec2 getScale ();
        void setScale (glm::vec2 scale);

        std::uint64_t getLayers ();
        void setLayers (std::uint64_t layers);

        std::uint64_t getMask ();
        void setMask (std::uint64_t mask);
    };

    template <>
    class ShapeRequest<BoxShape2DNew> {
    private:
        glm::vec2 scale;
        std::uint64_t layers;
        std::uint64_t mask;

    public:
        static constexpr std::size_t type = 1; // TODO: constants

        ShapeRequest (glm::vec2 scale, std::uint64_t layers, std::uint64_t mask) : scale{scale}, layers{layers}, mask{mask} {}

        BoxShape2DNew make (ColliderId collider);
    };
}