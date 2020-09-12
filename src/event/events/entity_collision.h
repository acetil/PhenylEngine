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
        component::EntityComponentManager* componentManager;
        EventBus* eventBus; // TODO: find better way
        EntityCollisionEvent () : entityId(0), otherId(0), collisionLayers(0), componentManager(nullptr), eventBus(nullptr) {}
        EntityCollisionEvent (int _entityId, int _otherId, unsigned int _collisionLayers,
                              component::EntityComponentManager* compManager, EventBus* bus) : entityId(_entityId),
                                    otherId(_otherId), collisionLayers(_collisionLayers), componentManager(compManager), eventBus(bus) {};
    };
}
#endif
