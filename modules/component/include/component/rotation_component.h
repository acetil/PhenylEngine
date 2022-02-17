#pragma once

#include "graphics/maths_headers.h"
#include "serialisable_component.h"

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
        RotationComponent& operator= (float newRot) {
            rotation = newRot;
            rotMatrix = {{glm::cos(newRot), glm::sin(newRot)}, {-glm::sin(newRot), glm::cos(newRot)}};
            return *this;
        }
        friend SerialisableComponent<RotationComponent>;
    };
}
