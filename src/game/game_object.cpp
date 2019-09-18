#include <string>
#include "game_object.h"
#include "graphics/graphics.h"
#include "logging/logging.h"
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
    if (entityRegistry.count(name) == 0) {
        logging::logf(LEVEL_WARNING, "Attempted creation of entity with name '%s' which doesn't exist!", name.c_str());
        return nullptr;
    } else {
        auto entity = entityRegistry[name]->createEntity(x, y);
        entity->setEntityId(currentEntityId);
        entities[currentEntityId++] = entity;
        logging::logf(LEVEL_DEBUG, "Created entity with name %s and id %d", name.c_str(), currentEntityId - 1);
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
    for (auto const& it : entities) {
        it.second->render(graphics);
    }
}
void game::GameObject::setTextureIds (graphics::Graphics* graphics) {
    for (auto const& it : entityRegistry) {
        it.second->setTextureIds(graphics);
    }
}