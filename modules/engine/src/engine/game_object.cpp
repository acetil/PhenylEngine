#include <string>
#include <utility>
#include <math.h>
#include <fstream>

#include "engine/game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "common/events/entity_creation.h"
#include "common/events/map_load.h"
#include "engine/map/map_reader.h"
#include "engine/phenyl_game.h"
#include "common/assets/assets.h"

using namespace game;

detail::GameObject::~GameObject () {
    for (auto& i : tileRegistry) {
        delete i;
    }
}

component::Entity detail::GameObject::makeDeserializedEntity (const nlohmann::json& serialized) {
    //auto entityView = entityComponentManager->create();
    if (!serialized.is_object()) {
        logging::log(LEVEL_ERROR, "Expected object for serialized entity!");
        return component::Null;
    }

    // TODO: refactor
    if (!serialized.contains("components")) {
        logging::log(LEVEL_ERROR, "Serialised entity did not contain components field!");
        return {};
    }

    if (serialized.contains("prefab") && serialized.at("prefab").is_string()) {
        auto prefab = common::Assets::Load<component::Prefab>(serialized.at("prefab"));
        if (!prefab) {
            logging::log(LEVEL_ERROR, "Failed to load prefab \"{}\"!", serialized.at("prefab").get<std::string>());
            return component::Null;
        }

        auto instantiator = prefab->instantiate();
        serializer->deserializeEntity(instantiator, serialized.at("components"));

        return instantiator.complete();
    } else {
        auto instantiator = component::Prefab::NullPrefab(entityComponentManager).instantiate({});
        serializer->deserializeEntity(instantiator, serialized.at("components"));
        return instantiator.complete();
    }
}

void detail::GameObject::registerTile (Tile* tile) {
    if (tileMap.count(tile->getName()) > 0) {
        logging::log(LEVEL_WARNING, "Duplicate registry of tile with name '{}'!", tile->getName());
        return;
    }
    int pos = tileRegistry.size();
    tileRegistry.push_back(tile);
    tileMap[tile->getName()] = pos;
}
int detail::GameObject::getTileId (const std::string& name) {
    return tileMap[name];
}
Tile* detail::GameObject::getTile (int tileId) {
    if (tileRegistry.size() < tileId || tileId < 0) {
        return nullptr;
    }
    return tileRegistry[tileId];
}
Tile* detail::GameObject::getTile (const std::string& name) {
    // convenience function
    return getTile(getTileId(name));
}

event::EventBus::SharedPtr detail::GameObject::getEventBus () {
    return eventBus;
}
void detail::GameObject::setEntityComponentManager (component::EntityComponentManager* manager) {
    this->entityComponentManager = manager;
}

void detail::GameObject::loadMap (Map::SharedPtr map) {
    entityComponentManager->clear();
    this->gameMap = std::move(map);

    for (auto& i : gameMap->getEntities()) {
        makeDeserializedEntity(i.data);
    }

    eventBus->raise(event::MapLoadEvent(gameMap));
}

void detail::GameObject::reloadMap () {
    loadMap(gameMap);
}

void detail::GameObject::mapReloadRequest (event::ReloadMapEvent& event) {
    reloadMap();
}

void detail::GameObject::updateCamera (graphics::Camera& _camera) {
    this->camera.updateCamera(_camera);
}

GameCamera& detail::GameObject::getCamera () {
    return camera;
}

void detail::GameObject::dumpMap (const std::string& filepath) {
    //util::DataArray entities;
    auto entities = nlohmann::json::array_t{};
    auto gameView = view::GameView(this);

    for (auto i : *entityComponentManager) {
        auto compDataObj = serializeEntity(i);


        entities.emplace_back(std::move(compDataObj));
    }

    logging::log(LEVEL_DEBUG, "Num in array: {}", entities.size());
    gameMap->writeMapJson(filepath, std::move(entities));
}

void detail::GameObject::mapDumpRequest (event::DumpMapEvent& event) {
    dumpMap(event.filepath);
}

void detail::GameObject::mapLoadRequest (event::MapLoadRequestEvent& event) {
    loadMap(readMap(event.filepath, PhenylGame(shared_from_this())));
}

void detail::GameObject::addEventHandlers (event::EventBus::SharedPtr _eventBus) {
    eventBus = std::move(_eventBus);
    gameInput.setEventBus(eventBus);
    eventScope = eventBus->getScope();
    eventBus->subscribe(&detail::GameObject::mapReloadRequest, this, eventScope);
    eventBus->subscribe(&detail::GameObject::mapDumpRequest, this, eventScope);
    eventBus->subscribe(&detail::GameObject::mapLoadRequest, this, eventScope);
}

GameInput& detail::GameObject::getInput () {
    return gameInput;
}

nlohmann::json detail::GameObject::serializeEntity (component::Entity& entity) {
    return serializer->serializeEntity(entity);
}

void detail::GameObject::setSerializer (component::EntitySerializer* serializer) {
    this->serializer = serializer;
}

void detail::GameObject::addDefaultSerialisers () {}
