#pragma once

#include "graphics/maths_headers.h"
#include "component/serialisable_component.h"

namespace physics {
    struct CollisionComponent : public component::SerialisableComponent<CollisionComponent> {
    private:
        static constexpr std::string_view name = "collision_comp";
        static constexpr std::string_view const& getName () {
            return name;
        }
        util::DataValue serialise ();
        void deserialise (const util::DataValue& val);
    public:
        glm::vec2 pos;
        glm::mat2 bbMap;
        float outerRadius;
        float mass;
        unsigned int layers = 0;
        unsigned int masks = 0;
        unsigned int resolveLayers = 0;
        unsigned int eventLayer = 0;
        friend component::SerialisableComponent<CollisionComponent>;
    };
}