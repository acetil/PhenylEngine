#pragma once

#include "component/detail/entity_id_list.h"
#include "component/entity_id.h"
#include "component/detail/relationships.h"
#include "component/detail/loggers.h"

namespace phenyl::component {
    class ComponentManager2 {
    private:
        detail::EntityIdList idList;
        detail::RelationshipManager relationships;

        void removeInt (EntityId id, bool updateParent) {
            if (updateParent) {
                // TODO: signal remove child
            }

            auto curr = relationships.entityChildren(id);
            while (curr) {
                auto next = relationships.next(curr);
                removeInt(curr, false);
                curr = next;
            }

            relationships.remove(id, updateParent);

            // TODO: delete components

            idList.removeId(id);
        }
    public:
        EntityId create (EntityId parent) {
            auto id = idList.newId();

            relationships.add(id, parent);
            // TODO: signals
            return id;
        }

        [[nodiscard]] bool exists (EntityId id) const noexcept {
            return idList.check(id);
        }

        void remove (EntityId id) {
            if (!idList.check(id)) {
                PHENYL_LOGE(detail::MANAGER_LOGGER, "Attempted to delete invalid entity {}!", id.value());
                return;
            }

            // TODO: defer

            removeInt(id, true);
        }

        [[nodiscard]] EntityId parent (EntityId id) const noexcept {
            return relationships.parent(id);
        }

        void reparent (EntityId id, EntityId parent) {
            // TODO: signals
            relationships.removeFromParent(id);
            relationships.setParent(id, parent);
        }
    };
}
