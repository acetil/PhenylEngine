#pragma once

#include "graphics/maths_headers.h"

namespace util {
    class DataValue;

#ifndef PHENYL_OPTIONAL_DECLARE
#define PHENYL_OPTIONAL_DECLARE
    template <typename T, typename = void>
    class Optional;
#endif
}

namespace component {
    class Position2D {
    private:
        glm::vec2 vec;
    public:
        Position2D () : vec{} {}
        explicit Position2D (glm::vec2 pos) : vec{pos} {}

        inline const glm::vec2& get () const {
            return vec;
        }

        inline Position2D& operator= (const glm::vec2& newVec) {
            vec = newVec;
            return *this;
        }

        inline Position2D& operator+= (const glm::vec2& other) {
            vec += other;
            return *this;
        }

        inline Position2D& operator-= (const glm::vec2& other) {
            vec -= other;
            return *this;
        }

        operator glm::vec2 () {
            return vec;
        }
    };

    util::DataValue serialisePos2D (const Position2D& pos2D);
    util::Optional<Position2D> deserialisePos2D (const util::DataValue& data);
}