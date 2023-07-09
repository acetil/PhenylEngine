#pragma once

#include "physics/shape/physics_shape.h"
#include "physics/physics.h"
#include "graphics/maths_headers.h"

namespace physics {
    class BoxShape2D;

    template <>
    class ShapeView<BoxShape2D> {
    private:
        BoxShape2D& shape;

        ShapeView (void* shapeData) : shape{*(static_cast<BoxShape2D*>(shapeData))} {}

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
    class ShapeRequest<BoxShape2D> {
    private:
        glm::vec2 scale;
        std::uint64_t layers;
        std::uint64_t mask;

    public:
        static constexpr std::size_t type = 1; // TODO: constants

        ShapeRequest (glm::vec2 scale, std::uint64_t layers, std::uint64_t mask) : scale{scale}, layers{layers}, mask{mask} {}

        BoxShape2D make (ColliderId collider);
    };
}