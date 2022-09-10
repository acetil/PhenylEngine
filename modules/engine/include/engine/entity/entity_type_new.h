#pragma once

#include <string>

#include "util/map.h"
#include "component/component.h"
#include "util/data.h"

namespace game {
    namespace detail {
        class ComponentFactory;
        class GameObject;
    }

    class EntityTypeNew {
    private:
        std::vector<detail::ComponentFactory> factories;
    public:
        EntityTypeNew ();
        explicit EntityTypeNew (std::vector<detail::ComponentFactory> factories);
        ~EntityTypeNew();

        EntityTypeNew (const EntityTypeNew&) = delete;
        EntityTypeNew& operator= (const EntityTypeNew&) = delete;

        EntityTypeNew (EntityTypeNew&&) noexcept;
        EntityTypeNew& operator= (EntityTypeNew&&) noexcept;

        void addDefaultComponents (component::view::EntityView& entityView) const;
    };

    struct EntityTypeComponent {
        std::string typeId;
    };

    EntityTypeNew makeEntityType (const util::DataValue& entityTypeData, detail::GameObject& gameObject);
}