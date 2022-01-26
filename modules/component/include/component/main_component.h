#include "graphics/maths_headers.h"
#include "serialisable_component.h"
#ifndef MAIN_COMPONENT_H
#define MAIN_COMPONENT_H
namespace component {
    struct EntityMainComponent : SerialisableComponent<EntityMainComponent> {
    private:
        static constexpr std::string_view name = "main_comp";
        util::DataValue serialise () const;
        void deserialise (const util::DataValue& val);
        static constexpr std::string_view const& getName () {
            return name;
        }
    public:
        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec2 acc;
        float constFriction;
        float linFriction;
        friend SerialisableComponent<EntityMainComponent>;
    };
}
#endif