#pragma once

#include "graphics/maths_headers.h"

namespace component {
    /*class Position2D {
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
    };*/

    //util::DataValue phenyl_to_data (const Position2D& comp);
    //bool phenyl_from_data (const util::DataValue& dataVal, Position2D& comp);
}