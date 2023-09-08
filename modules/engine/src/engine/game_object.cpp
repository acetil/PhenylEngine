#include <string>
#include <utility>
#include <math.h>
#include <fstream>

#include "engine/game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "common/events/entity_creation.h"
#include "engine/entity/controller/entity_controller.h"
#include "common/events/map_load.h"
#include "engine/map/map_reader.h"
#include "engine/phenyl_game.h"
#include "component/component_serialiser.h"
#include "engine/entity/entity_type.h"

using namespace game;

detail::GameObject::~GameObject () {
    for (auto& i : tileRegistry) {
        delete i;
    }
}

util::Optional<component::Entity> detail::GameObject::createNewEntityInstance (const std::string& name, const util::DataValue& data) {
    // TODO: requires refactor
    /*if (!entityTypes.contains(name)) {
        logging::log(LEVEL_WARNING, "Attempted creation of entity with entity type \"{}\" which doesn't exist!", name);
        return util::NullOpt;
    } else {
        auto entityView = entityComponentManager->create();
        const auto& entityType = entityTypes.at(name);
        entityType.addDefaultComponents(entityView);

        auto gameView = view::GameView(this);

        entityView.get<EntityControllerComponent>().ifPresent([&entityView, &gameView, &data] (EntityControllerComponent& comp) {
            comp.get().initEntity(entityView, gameView, data);
        });

        eventBus->raise(event::EntityCreationEvent(*entityComponentManager, entityView, gameView));
        return {entityView};
    }*/
    if (!prefabs.contains(name)) {
        logging::log(LEVEL_WARNING, "Attempted creation of entity with entity type \"{}\" which doesn't exist!", name);
        return util::NullOpt;
    } else {
        return prefabs.at(name)
            .instantiate()
            .complete();
    }
}

component::Entity detail::GameObject::makeDeserializedEntity (const nlohmann::json& serialized) {
    //auto entityView = entityComponentManager->create();
    if (!serialized.is_object()) {
        logging::log(LEVEL_ERROR, "Expected object for serialized entity!");
        return component::Null;
    }
    //const auto& serialisedObj = serialized.get<nlohmann::json::object_t>();

    /*serialiser->deserialiseObject(entityView, serialisedObj);

    entityView.get<EntityTypeComponent>().ifPresent([this, &entityView] (EntityTypeComponent& entityType) {
        if (entityTypes.contains(entityType.typeId)) {
            entityTypes.at(entityType.typeId).addDefaultComponents(entityView);
        } else {
            logging::log(LEVEL_WARNING, "Unknown entity type: {}!", entityType.typeId);
        }
    });

    auto gameView = view::GameView(this);*/

    /*entityView.get<EntityControllerComponent>().ifPresent([&entityView, &gameView, &serialisedObj] (EntityControllerComponent& comp) {
        comp.get().initEntity(entityView, gameView, serialisedObj["data"]);
    });*/

    //eventBus->raise(event::EntityCreationEvent(*entityComponentManager, entityView, gameView));

    // TODO: refactor
    if (!serialized.contains("components")) {
        logging::log(LEVEL_ERROR, "Serialised entity did not contain components field!");
        return {};
    }

    if (serialized.contains("prefab") && serialized.at("prefab").is_string()) {
        auto& prefab = prefabs.at(serialized.at("prefab").get<std::string>());
        auto instantiator = prefab.instantiate();
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
void detail::GameObject::updateEntityPosition () {
    logging::log(LEVEL_ERROR, "Do not call this function!");
    //physics::updatePhysics(entityComponentManager);
    //physics::checkCollisions(entityComponentManager, eventBus, view::GameView(this));
}

void detail::GameObject::updateEntitiesPrePhysics () {
    // TODO: make better way
    auto gameView = view::GameView(this);
    controlEntitiesPrePhysics(*entityComponentManager, gameView, eventBus);
}

void detail::GameObject::updateEntitiesPostPhysics () {
    // TODO: make better way
    auto gameView = view::GameView(this);
    controlEntitiesPostPhysics(*entityComponentManager, gameView, eventBus);
}

void detail::GameObject::deleteEntityInstance (component::EntityId entityId) {
    //entityComponentManager->remove(entityId); // TODO: implement queue
    entityComponentManager->entity(entityId).remove();
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

void detail::GameObject::addEntityType (const std::string& typeId, const std::string& filepath) {
    if (entityTypes.contains(typeId)) {
        logging::log(LEVEL_WARNING, "Attempted to insert duplicate entity type \"{}\"!", typeId);
        return;
    }

    //auto data = util::parseFromFile(filepath);
    nlohmann::json data;
    std::ifstream file{filepath};
    file >> data;
    if (!data.empty()) {
        //entityTypes[typeId] = std::move(makeEntityType(data, *serialiser));
        //entityTypes[typeId] =
        makePrefab(data).ifPresent([this, &typeId] (component::Prefab& prefab) {
           prefabs[typeId] = std::move(prefab);
        });
    }
}

void detail::GameObject::setSerializer (component::EntitySerializer* serializer) {
    this->serializer = serializer;
}

void detail::GameObject::addDefaultSerialisers () {
   /* serialiser->addComponentSerialiser<EntityTypeComponent>("EntityType", [](const EntityTypeComponent& comp) -> util::DataValue {
        return (util::DataValue)comp.typeId;
    }, [](const util::DataValue& val) -> util::Optional<EntityTypeComponent> {
        if (val.is<std::string>()) {
            return {EntityTypeComponent{val.get<std::string>()}};
        } else {
            return util::NullOpt;
        }
    });

    serialiser->addComponentSerialiser<EntityControllerComponent>("Controller", [] (const EntityControllerComponent& comp) -> util::DataValue {
        return (util::DataValue)comp.get().getEntityId();
    }, [this] (const util::DataValue& val) -> util::Optional<EntityControllerComponent> {
        if (!val.is<std::string>()) {
            logging::log(LEVEL_ERROR, "Controller id must be a string!");
            return util::NullOpt;
        }

        auto& controllerId = val.get<std::string>();

        auto controller = getController(controllerId);
        return controller.thenMap([](EntityController* controller) {
            return EntityControllerComponent{controller};
        });
    });*/
}

void detail::GameObject::registerEntityController (std::unique_ptr<EntityController> controller) {
    if (controllers.contains(controller->getEntityId())) {
        logging::log(LEVEL_WARNING, "Attempted to register entity controller with duplicate id \"{}\"!", controller->getEntityId());
        return;
    }

    controllers[controller->getEntityId()] = std::move(controller);
}

util::Optional<EntityController*> detail::GameObject::getController (const std::string& name) {
    if (controllers.contains(name)) {
        return util::Optional<EntityController*>{controllers[name].get()};
    } else {
        return util::NullOpt;
    }
}

util::Optional<component::Prefab> detail::GameObject::makePrefab (const nlohmann::json& val) {
    if (!val.is_object()) {
        return util::NullOpt;
    }

    if (!val.contains("components")) {
        logging::log(LEVEL_ERROR, "Prefab does not contain components field!");
        return util::NullOpt;
    }

    auto builder = entityComponentManager->buildPrefab();
    const auto& components = val.at("components");
    serializer->deserializePrefab(builder, components);

    return {builder.build()};
}

void detail::GameObject::clearPrefabs () {
    prefabs.clear();
}
