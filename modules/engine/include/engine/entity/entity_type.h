#pragma once

#include <string>

#include "util/map.h"
#include "component/forward.h"
#include "component/component.h"
#include "util/data.h"

namespace game {
    class EntityType {
    private:
        std::vector<component::EntityComponentFactory> factories;
    public:
        EntityType ();
        explicit EntityType (std::vector<component::EntityComponentFactory> factories);
        ~EntityType();

        EntityType (const EntityType&) = delete;
        EntityType& operator= (const EntityType&) = delete;

        EntityType (EntityType&&) noexcept;
        EntityType& operator= (EntityType&&) noexcept;

        void addDefaultComponents (component::Entity& entityView) const;
    };

    struct EntityTypeComponent {
        std::string typeId;
    };

    EntityType makeEntityType (const util::DataValue& entityTypeData, component::EntitySerialiser& serialiser);
}