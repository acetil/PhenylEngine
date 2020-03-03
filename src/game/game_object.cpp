#include <string>
#include "game_object.h"
#include "graphics/graphics.h"
#include "logging/logging.h"
#include "physics/physics_new.h"
#include "event/events/entity_creation.h"
#include "entity/controller/entity_controller.h"

using namespace game;

game::GameObject::~GameObject () {
    for (auto const& x : entityRegistry) {
        delete x.second;
    }
    for (auto const& x : entities) {
        delete x.second;
    }
    for (int i = 0; i < tileRegistry.size(); i++) {
        delete tileRegistry[i];
    }
    delete eventBus;
}

void game::GameObject::registerEntity (std::string name, AbstractEntity* entity) {
    if (entityRegistry.count(name) > 0) {
        logging::logf(LEVEL_WARNING, "Duplicate registry of entity with name '%s'!", name.c_str());
        return;
    }
    entityRegistry[name] = entity;
    logging::logf(LEVEL_INFO, "Registered entity with name %s", name.c_str());
}
AbstractEntity* game::GameObject::getEntity (std::string name) {
    if (entityRegistry.count(name) > 0) {
        return entityRegistry[name];
    } else {
        return nullptr;
    }
}
AbstractEntity* game::GameObject::createNewEntityInstance (std::string name, float x, float y) {
    // TODO: requires refactor
    if (entityRegistry.count(name) == 0) {
        logging::logf(LEVEL_WARNING, "Attempted creation of entity with name '%s' which doesn't exist!", name.c_str());
        return nullptr;
    } else {
        auto entity = entityRegistry[name]->createEntity();
        int entityId = entityComponentManager->addObject(entity);
        entity->setEntityId(entityId);
        component::EntityMainComponent comp = entityComponentManager->getObjectData<component::EntityMainComponent>(entityComponentManager->getComponentId("main_component"), entityId);
        entity->x = comp.pos;
        entity->y = comp.pos + 1;
        auto uvPtr = entityComponentManager->getObjectDataPtr<float>(entityComponentManager->getComponentId("uv"), entityId);
        eventBus->raiseEvent(new event::EntityCreationEvent(x, y, entity->scale, entityComponentManager, entity, entityId));
        logging::logf(LEVEL_DEBUG, "Created entity with name %s and id %d", name.c_str(), entityId);
        return entity;
    }
}
AbstractEntity* game::GameObject::getEntityInstance (int entityId) {
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
}

void game::GameObject::registerTile (Tile* tile) {
    if (tileMap.count(tile->getName()) > 0) {
        logging::logf(LEVEL_WARNING, "Duplicate registry of tile with name '%s'!", tile->getName().c_str());
        return;
    }
    int pos = tileRegistry.size();
    tileRegistry.push_back(tile);
    tileMap[tile->getName()] = pos;
}
int game::GameObject::getTileId (std::string name) {
    return tileMap[name];
}
Tile* game::GameObject::getTile (int tileId) {
    if (tileRegistry.size() < tileId || tileId < 0) {
        return NULL;
    }
    return tileRegistry[tileId];
}
Tile* game::GameObject::getTile (std::string name) {
    // convenience function
    return getTile(getTileId(name));
}

void game::GameObject::updateEntities (float deltaTime) {
    for (auto const& it : entities) {
        it.second->update(deltaTime);
    }
}
void game::GameObject::updateEntityPositions (float deltaTime) {
    for (auto const& it : entities) {
        it.second->updatePosition(deltaTime);
    }
}
void game::GameObject::renderEntities (graphics::Graphics* graphics) {
    /*for (auto const& it : entities) {
        it.second->render(graphics);
    }*/
    // TODO: update to remove magic numbers (12 is uv components per sprite)
    entityComponentManager->applyFunc<component::EntityMainComponent, graphics::Graphics*>([](component::EntityMainComponent* comp, int numSprites, int direction, graphics::Graphics* graphics){graphics->bufferEntityPositions(comp, numSprites, direction, graphics->getSpriteBuffer());}, 1, graphics);
    entityComponentManager->applyFunc<float, graphics::Graphics*>([](float *uv, int numEntities, int direction, graphics::Graphics* graphics){
        graphics::Buffer* buf = graphics->getSpriteBuffer(); 
        for (int i = 0; i < numEntities; i++) {
            float* uvPos = buf->getUvBufferPos();
            for (int j = 0; j < 12; j++) {
                *(uvPos++) = uv[i * 12 + j];     
            }
        }
    }, 2, graphics);
}
void game::GameObject::setTextureIds (graphics::Graphics* graphics) {
    for (auto const& it : entityRegistry) {
        it.second->setTextureIds(graphics);
    }
}
event::EventBus* game::GameObject::getEventBus () {
    return eventBus;
}
void game::GameObject::setEntityComponentManager (component::ComponentManager<AbstractEntity*>* manager) {
    this->entityComponentManager = manager;
}
void game::GameObject::updateEntityPosition () {
    entityComponentManager->applyFunc(physics::updatePhysics, 1); // TODO: remove hardcode (1 is index of main comp)
}
void entityPrePhysicsFunc (AbstractEntity** entities, int startId, int numEntities, int direction, int size, component::ComponentManager<AbstractEntity*>* manager) {
    auto comp = manager->getComponent<component::EntityMainComponent>(1);
    controlEntitiesPrePhysics(entities, comp, startId, numEntities, direction, manager);
}
void entityPostPhysicsFunc (AbstractEntity** entities, int startId, int numEntities, int direction, int size, component::ComponentManager<AbstractEntity*>* manager) {
    auto comp = manager->getComponent<component::EntityMainComponent>(1);
    controlEntitiesPostPhysics(entities, comp, startId, numEntities, direction, manager);
}
void game::GameObject::updateEntitiesPrePhysics () {
    entityComponentManager->applyFunc(entityPrePhysicsFunc, 0, entityComponentManager);
}

void GameObject::updateEntitiesPostPhysics () {
    entityComponentManager->applyFunc(entityPostPhysicsFunc, 0, entityComponentManager);
}
