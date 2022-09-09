#pragma once

#include "graphics/maths_headers.h"
#include "serialisable_component.h"

namespace component {
    struct FrictionKinematicsMotion2D : SerialisableComponent<FrictionKinematicsMotion2D> {
    private:
        static constexpr std::string_view name = "main_comp";
        util::DataValue serialise () const;
        void deserialise (const util::DataValue& val);
        static constexpr std::string_view const& getName () {
            return name;
        }
    public:
        //glm::vec2 pos;
        glm::vec2 velocity;
        glm::vec2 acceleration;
        float constFriction;
        float linFriction;

        friend SerialisableComponent<FrictionKinematicsMotion2D>;
    };
}
