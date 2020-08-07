#include "component/component.h"
#include "event/event.h"
#include <string>
#ifndef ENTITIY_COLLISION_H
#define ENTITIY_COLLISION_H
namespace event {
    class EntityCollisionEvent : public Event<EntityCollisionEvent> {
    public:
        int entityId;
        int otherId;
        unsigned int collisionLayers;
        std::string name = "entity_collision";
        component::ComponentManager<game::AbstractEntity*>* componentManager;
        EntityCollisionEvent () : entityId(0), otherId(0), collisionLayers(0), componentManager(nullptr) {}
        EntityCollisionEvent (int _entityId, int _otherId, unsigned int _collisionLayers,
                              component::ComponentManager<game::AbstractEntity*>* compManager) : entityId(_entityId),
                                    otherId(_otherId), collisionLayers(_collisionLayers), componentManager(compManager) {};
    };
}
#endif
