#include <string>
#include "game_object.h"
#include "graphics/graphics.h"
using namespace game;

void game::GameObject::registerEntity (std::string name, AbstractEntity* entity) {
    entityRegistry[name] = entity;
}
AbstractEntity* game::GameObject::getEntity (std::string name) {
    if (entityRegistry.count(name) > 0) {
        return entityRegistry[name];
    } else {
        return nullptr;
    }
}
AbstractEntity* game::GameObject::createNewEntityInstance (std::string name, float x, float y) {
    if (entityRegistry.count(name) == 0) {
        return nullptr;
    } else {
        auto entity = entityRegistry[name]->createEntity(x, y);
        entity->setEntityId(currentEntityId);
        entities[currentEntityId++] = entity;
        return entity;
    }
}
AbstractEntity* game::GameObject::getEntityInstance (int entityId) {
    if (entities.count(entityId) > 0) {
        return entities[entityId];
    } else {
        return nullptr;
    }
}
void game::GameObject::deleteEntityInstance (AbstractEntity* entity) {
    int entityId = entity->getEntityId();
    delete entity;
    entities[entityId] = nullptr;
    entities.erase(entityId);
}
void game::GameObject::deleteEntityInstance (int entityId) {
    deleteEntityInstance(entities[entityId]);
}
void game::GameObject::updateEntities (float deltaTime) {
    for (auto const& it : entities) {
        it.second->update(deltaTime);
    }
}
void game::GameObject::updateEntityPosition (float deltaTime) {
    for (auto const& it : entities) {
        it.second->updatePosition(deltaTime);
    }
}
void game::GameObject::renderEntities (graphics::Graphics* graphics) {
    for (auto const& it : entities) {
        it.second->render(graphics);
    }
}