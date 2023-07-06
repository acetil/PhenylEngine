#pragma once

#include <cstdint>
#include <concepts>

#include "graphics/maths_headers.h"
#include "physics/physics.h"

namespace physics {

    class Shape2DNew {
    private:
        ColliderId collider;
        float outerRadius;
        std::uint64_t layers;
        std::uint64_t mask;
        glm::vec2 position{};
    protected:
        void setOuterRadius (float newOuterRadius);
    public:
        Shape2DNew (ColliderId collider, float outerRadius, std::uint64_t layers, std::uint64_t mask);

        std::uint64_t getLayers () const; // TODO: remove?
        std::uint64_t getMask () const; // TODO: remove?

        void setLayers (std::uint64_t newLayers);
        void setMask (std::uint64_t newMask);

        ColliderId getColliderId () const;

        bool shouldCollide (const Shape2DNew& other) const;

        virtual void applyTransform (glm::mat2 transform) = 0;
        void setPosition (glm::vec2 position);

        glm::vec2 getDisplacement (const Shape2DNew& other) const;
        virtual util::DataValue serialise () const = 0;
    };

    template <typename T>
    concept good_shape_2d = requires (T t) {
        requires std::derived_from<T, Shape2DNew>;
        { t.applyTransform(std::declval<glm::mat2>()) } -> std::same_as<void>;
    };
}