#pragma once

#include "graphics/maths_headers.h"
#include "component/serialisable_component.h"

namespace physics {
    struct CollisionComponent2D : public component::SerialisableComponent<CollisionComponent2D> {
    private:
        static constexpr std::string_view name = "collision_comp";
        static constexpr std::string_view const& getName () {
            return name;
        }
        util::DataValue serialise () const;
        void deserialise (const util::DataValue& val);
    public:
        //glm::vec2 pos;
        glm::mat2 bbMap;
        glm::mat2 rotBBMap;
        float outerRadius;
        float mass;
        unsigned int layers = 0;
        unsigned int masks = 0;
        unsigned int resolveLayers = 0;
        unsigned int eventLayer = 0;
        friend component::SerialisableComponent<CollisionComponent2D>;
    };
}
