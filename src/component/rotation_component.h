#include "graphics/maths_headers.h"
#include "serialisable_component.h"
#ifndef ROTATION_COMPONENT_H
#define ROTATION_COMPONENT_H
namespace component {
    struct RotationComponent : SerialisableComponent<RotationComponent> {
    private:
        static constexpr std::string_view name = "rotation";
        util::DataValue serialise () const;
        void deserialise (const util::DataValue& val);
        static constexpr std::string_view const& getName () {
            return name;
        }
    public:
        glm::mat2 rotMatrix;
        float rotation;
        friend SerialisableComponent<RotationComponent>;
    };
}
#endif
