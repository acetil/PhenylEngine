#include <string>
#include <math.h>

#include "game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "event/events/entity_creation.h"
#include "entity/controller/entity_controller.h"
#include "component/rotation_update.h"


using namespace game;

game::GameObject::~GameObject () {
    for (auto const& x : entityRegistry) {
        delete x.second;
    }
    /*for (auto const& x : entities) {
        delete x.second;
    }*/
    for (auto& i : tileRegistry) {
        delete i;
    }
    delete eventBus;
}

void game::GameObject::registerEntity (const std::string& name, AbstractEntity* entity) {
    if (entityRegistry.count(name) > 0) {
        logging::logf(LEVEL_WARNING, "Duplicate registry of entity with name '%s'!", name.c_str());
        return;
    }
    entityRegistry[name] = entity;
    logging::logf(LEVEL_INFO, "Registered entity with name %s", name.c_str());
}

[[maybe_unused]] AbstractEntity* game::GameObject::getEntity (const std::string& name) {
    if (entityRegistry.count(name) > 0) {
        return entityRegistry[name];
    } else {
        return nullptr;
    }
}
AbstractEntity* game::GameObject::createNewEntityInstance (const std::string& name, float x, float y) {
    // TODO: requires refactor
    if (entityRegistry.count(name) == 0) {
        logging::logf(LEVEL_WARNING, "Attempted creation of entity with name '%s' which doesn't exist!", name.c_str());
        return nullptr;
    } else {
        auto entity = entityRegistry[name]->createEntity();
        int entityId = entityComponentManager->addObject(entity);
        entity->setEntityId(entityId);
        auto comp = entityComponentManager->getObjectData<component::EntityMainComponent>(entityId);
        entity->x = &comp.pos.x;
        entity->y = &comp.pos.y;
        //auto uvPtr = entityComponentManager->getObjectDataPtr<float>(entityComponentManager->getComponentId("uv"), entityId);
        auto event = event::EntityCreationEvent(x, y, entity->scale, entityComponentManager, entity, entityId);
        event.eventBus = eventBus;
        //eventBus->raiseEvent(event::EntityCreationEvent(x, y, entity->scale, entityComponentManager, entity, entityId));
        eventBus->raiseEvent(event);
        logging::logf(LEVEL_DEBUG, "Created entity with name %s and id %d", name.c_str(), entityId);
        return entity;
    }
}
/*AbstractEntity* game::GameObject::getEntityInstance (int entityId) {
    if (entities.count(entityId) > 0) {
        return entities[entityId];
    } else {
        return NULL;
    }
}
void game::GameObject::deleteEntityInstance (AbstractEntity* entity) {
    int entityId = entity->getEntityId();
    delete entity;
    entities[entityId] = NULL;
    entities.erase(entityId);
}
void game::GameObject::deleteEntityInstance (int entityId) {
    deleteEntityInstance(entities[entityId]);
}*/

void game::GameObject::registerTile (Tile* tile) {
    if (tileMap.count(tile->getName()) > 0) {
        logging::logf(LEVEL_WARNING, "Duplicate registry of tile with name '%s'!", tile->getName().c_str());
        return;
    }
    int pos = tileRegistry.size();
    tileRegistry.push_back(tile);
    tileMap[tile->getName()] = pos;
}
int game::GameObject::getTileId (const std::string& name) {
    return tileMap[name];
}
Tile* game::GameObject::getTile (int tileId) {
    if (tileRegistry.size() < tileId || tileId < 0) {
        return nullptr;
    }
    return tileRegistry[tileId];
}
Tile* game::GameObject::getTile (const std::string& name) {
    // convenience function
    return getTile(getTileId(name));
}

/*void game::GameObject::updateEntities (float deltaTime) {
    for (auto const& it : entities) {
        it.second->update(deltaTime);
    }
}*/
void game::GameObject::setTextureIds (graphics::TextureAtlas atlas) {
    for (auto const& it : entityRegistry) {
        it.second->setTextureIds(atlas);
    }
}
event::EventBus* game::GameObject::getEventBus () {
    return eventBus;
}
void game::GameObject::setEntityComponentManager (component::EntityComponentManager* manager) {
    this->entityComponentManager = manager;
}
void game::GameObject::updateEntityPosition () {
    physics::updatePhysics(entityComponentManager);
    physics::checkCollisions(entityComponentManager, eventBus);
}
void entityPrePhysicsFunc (AbstractEntity** entities, int numEntities, int direction, component::EntityComponentManager* manager, event::EventBus* bus) {
    controlEntitiesPrePhysics(manager, 0, numEntities, direction, bus);
}
void entityPostPhysicsFunc (AbstractEntity** entities, int numEntities, int direction, component::EntityComponentManager* manager, event::EventBus* bus) {
    controlEntitiesPostPhysics(manager, 0, numEntities, direction, bus);
}
void game::GameObject::updateEntitiesPrePhysics () {
    // TODO: make better way
    entityComponentManager->applyFunc<AbstractEntity*>(entityPrePhysicsFunc, entityComponentManager, eventBus);
}

void GameObject::updateEntitiesPostPhysics () {
    // TODO: make better way
    entityComponentManager->applyFunc<AbstractEntity*>(entityPostPhysicsFunc, entityComponentManager, eventBus);
}
