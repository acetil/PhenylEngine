#include <string>
#include <utility>
#include <math.h>

#include "engine/game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "common/events/entity_creation.h"
#include "engine/entity/controller/entity_controller.h"
#include "common/events/map_load.h"
#include "engine/map/map_reader.h"
#include "engine/phenyl_game.h"
#include "component/position.h"
#include "component/rotation_component.h"
#include "component/component_serialiser.h"

using namespace game;

detail::GameObject::~GameObject () {
    /*for (auto const& x : entities) {
        delete x.second;
    }*/
    for (auto& i : tileRegistry) {
        delete i;
    }
    //delete eventBus;
}

/*void game::GameObject::registerEntity (const std::string& name, AbstractEntity* entity) {
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
}*/
component::EntityView detail::GameObject::createNewEntityInstance (const std::string& name, const util::DataValue& data) {
    // TODO: requires refactor
    if (!entityTypesNew.contains(name)) {
        logging::log(LEVEL_WARNING, "Attempted creation of entity with entity type \"{}\" which doesn't exist!", name);
        return {{0, 0}, nullptr};
    } else {
        auto entityView = entityComponentManager->createEntity();
        //auto entityView = entityComponentManager->getEntityView(entityId);
        const auto& entityType = entityTypesNew.at(name);
        entityType.addDefaultComponents(entityView);

        //entityView.addComponent<component::Position2D>(component::Position2D{});
        //entityView.addComponent<component::Rotation2D>(component::Rotation2D{});

        //entityView.getComponent<component::Position2D>().getUnsafe() = {x, y};
        //entityView.getComponent<component::Rotation2D>().getUnsafe() = rot;


        //setInitialEntityValues(entityComponentManager, entityTypes[name], entityId, x, y, rot);
        //auto viewCore = view::ViewCore(entityComponentManager);
        auto gameView = view::GameView(this);

        entityView.getComponent<std::shared_ptr<EntityController>>().ifPresent([&entityView, &gameView, &data] (std::shared_ptr<EntityController>& controller) {
            controller->initEntity(entityView, gameView, data);
        });

        //entityView.getComponent<std::shared_ptr<EntityController>>().getUnsafe()->initEntity(entityView, gameView, data);
        eventBus->raise(event::EntityCreationEvent(entityComponentManager, entityView, gameView));
        return entityView;
    }
}

component::EntityView detail::GameObject::makeDeserialisedEntity (const util::DataValue& serialised) {
    auto entityView = entityComponentManager->createEntity();
    auto serialisedObj = serialised.get<util::DataObject>();
    //setInitialEntityValues(entityComponentManager, entityTypes.at(type), entityId, x, y, rot);


    //component::deserialiseComps<ENTITY_LIST>(entityComponentManager, entityId, serialisedObj);


    //auto viewCore = view::ViewCore(entityComponentManager);
    //auto entityView = view::EntityView(viewCore, entityId, eventBus);
    //auto entityView = entityComponentManager->getEntityView(entityId);
    //const auto& entityType = entityTypes.at(type);
    //entityView.addComponent<EntityType>(entityType);
    //entityView.addComponent<AbstractEntity*>(entityType.entityFactory());
    //entityView.addComponent<std::shared_ptr<EntityController>>(entityType.defaultController);

    deserialiseEntity2(entityView, serialisedObj);

    entityView.getComponent<EntityTypeComponent>().ifPresent([this, &entityView] (EntityTypeComponent& entityType) {
        if (entityTypesNew.contains(entityType.typeId)) {
            entityTypesNew.at(entityType.typeId).addDefaultComponents(entityView);
        } else {
            logging::log(LEVEL_WARNING, "Unknown entity type: {}!", entityType.typeId);
        }
    });

    auto gameView = view::GameView(this);

    //entityView.

    entityView.getComponent<std::shared_ptr<EntityController>>().ifPresent([&entityView, &gameView, &serialisedObj](std::shared_ptr<EntityController>& controller){
        controller->initEntity(entityView, gameView, serialisedObj["data"]);
    });

    eventBus->raise(event::EntityCreationEvent(entityComponentManager, entityView, gameView));
    /*entityView.getComponent<component::Position2D>().ifPresent([&entityView, &gameView, this, &entityId] (component::Position2D& pos) {
       eventBus->raise(event::EntityCreationEvent(entityComponentManager, nullptr, entityId, entityView, gameView));
    });*/
    /*eventBus->raise(event::EntityCreationEvent(x, y, 0.1f, entityComponentManager,
                                               entityComponentManager->getObjectData<AbstractEntity*>(entityId).orElse(
                                                       nullptr), entityId,
                                               entityView, gameView));*/
    return entityView;
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

/*void game::GameObject::updateEntities (float deltaTime) {
    for (auto const& it : entities) {
        it.second->update(deltaTime);
    }
}*/
/*void detail::GameObject::setTextureIds (graphics::TextureAtlas& atlas) {
    for (auto const& it : controllers) {
        it.second->setTextureIds(atlas);
    }
}*/
event::EventBus::SharedPtr detail::GameObject::getEventBus () {
    return eventBus;
}
void detail::GameObject::setEntityComponentManager (component::EntityComponentManager::SharedPtr manager) {
    this->entityComponentManager = std::move(manager);
}
void detail::GameObject::updateEntityPosition () {
    physics::updatePhysics(entityComponentManager);
    physics::checkCollisions(entityComponentManager, eventBus, view::GameView(this));
}
/*void entityPrePhysicsFunc (AbstractEntity** entities, int numEntities, int direction, component::EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& bus, view::GameView gameView) {
    controlEntitiesPrePhysics(manager, gameView, 0, numEntities, direction, bus);
}
void entityPostPhysicsFunc (AbstractEntity** entities, int numEntities, int direction, component::EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& bus, view::GameView gameView) {
    controlEntitiesPostPhysics(manager, gameView, 0, numEntities, direction, bus);
}*/
void detail::GameObject::updateEntitiesPrePhysics () {
    // TODO: make better way
    auto gameView = view::GameView(this);
    controlEntitiesPrePhysics(entityComponentManager, gameView, eventBus);
    //entityComponentManager->applyFunc<AbstractEntity*>(entityPrePhysicsFunc, entityComponentManager, eventBus, gameView);
}

void detail::GameObject::updateEntitiesPostPhysics () {
    // TODO: make better way
    auto gameView = view::GameView(this);
    //entityComponentManager->applyFunc<AbstractEntity*>(entityPostPhysicsFunc, entityComponentManager, eventBus, gameView);
    controlEntitiesPostPhysics(entityComponentManager, gameView, eventBus);
}

void detail::GameObject::deleteEntityInstance (component::EntityId entityId) {
    entityComponentManager->removeEntity(entityId); // TODO: implement queue
}

std::shared_ptr<EntityController> detail::GameObject::getController (const std::string& name) {
    return controllers[name];
}

void detail::GameObject::loadMap (Map::SharedPtr map) {
    entityComponentManager->clear();
    this->gameMap = std::move(map);

    for (auto& i : gameMap->getEntities()) {
        //createNewEntityInstance(i.entityType, i.x, i.y, i.rotation, i.data);
        makeDeserialisedEntity(i.data);
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
    util::DataArray entities;
    auto gameView = view::GameView(this);
    auto ids = entityComponentManager->getComponent<component::EntityId>().orElse(nullptr);
    for (int i = 0; i < entityComponentManager->getNumObjects(); i++) {
        //auto compData = component::serialise<ENTITY_LIST>(entityComponentManager, ids[i]);
        auto view = entityComponentManager->getEntityView(ids[i]);
        auto compDataObj = serialiseEntity(view);
        auto compData = util::DataValue{std::move(compDataObj)};
        //auto entityView = view::EntityView(view::ViewCore(entityComponentManager), ids[i], eventBus);
        auto entityView = entityComponentManager->getEntityView(ids[i]);
        compData.get<util::DataObject>()["data"] =
                entityComponentManager->getObjectData<std::shared_ptr<game::EntityController>>(ids[i]).orElse(nullptr)->getData(entityView, gameView);
        //compData.get<util::DataObject>()["type"] = entityComponentManager->getObjectData<game::EntityType>(ids[i]).orElse(game::EntityType()).typeName;
        entities.push_back(compData);
        //compData.get<util::DataObject>()[""]
    }
    logging::log(LEVEL_DEBUG, "Num in array: {}", entities.size());
    gameMap->writeMapJson(filepath, (util::DataValue)entities);
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

void detail::GameObject::addComponentSerialiser (const std::string& component, std::unique_ptr<ComponentSerialiser> serialiser) {
    serialiserMap[component] = std::move(serialiser);
}

void detail::GameObject::deserialiseEntity2 (component::EntityView& entityView, const util::DataValue& entityData) {
    if (!entityData.is<util::DataObject>()) {
        logging::log(LEVEL_DEBUG, "Not object!");
        return;
    }
    logging::log(LEVEL_DEBUG, "Entity id: {}", entityView.getId().value());
    logging::log(LEVEL_DEBUG, entityData.toString());
    const auto& dataObj = entityData.get<util::DataObject>();

    for (const auto& [compId, serialiser] : serialiserMap.kv()) {
        if (dataObj.contains(compId)) {
            serialiser->deserialiseComp(entityView, dataObj.at(compId));
        }
    }
}

util::DataObject detail::GameObject::serialiseEntity (component::EntityView& entityView) {
    util::DataObject entityData;

    for (const auto& [compId, serialiser] : serialiserMap.kv()) {
        util::DataValue val = serialiser->serialiseComp(entityView);
        if (!val.empty()) {
            entityData[compId] = std::move(val);
        }
    }

    return entityData;
}

detail::ComponentSerialiser* detail::GameObject::getSerialiser (const std::string& component) {
    if (serialiserMap.contains(component)) {
        return serialiserMap[component].get();
    } else {
        return nullptr;
    }
}

void detail::GameObject::addEntityType (const std::string& typeId, const std::string& filepath) {
    if (entityTypesNew.contains(typeId)) {
        logging::log(LEVEL_WARNING, "Attempted to add duplicate entity type \"{}\"!", typeId);
        return;
    }

    auto data = util::parseFromFile(filepath);
    if (!data.empty()) {
        entityTypesNew[typeId] = std::move(makeEntityType(data, *this));
    }
}

void detail::GameObject::setSerialiser (component::EntitySerialiser* serialiser) {
    this->serialiser = serialiser;
}

void detail::GameObject::addDefaultSerialisers () {
    serialiser->addComponentSerialiser<EntityTypeComponent>("type", [](const EntityTypeComponent& comp) -> util::DataValue {
        return (util::DataValue)comp.typeId;
    }, [](const util::DataValue& val) -> util::Optional<EntityTypeComponent> {
        if (val.is<std::string>()) {
            return {EntityTypeComponent{val.get<std::string>()}};
        } else {
            return util::NullOpt;
        }
    });

    serialiser->addComponentSerialiser<std::shared_ptr<EntityController>>("controller", [] (const std::shared_ptr<EntityController>& comp) -> util::DataValue {
        return (util::DataValue)"TODO"; // TODO
    }, [this](const util::DataValue& val) -> util::Optional<std::shared_ptr<EntityController>> {
        if (!val.is<std::string>()) {
            return util::NullOpt;
        }

        auto& controllerId = val.get<std::string>();

        auto controller = getController(controllerId);
        if (controller) {
            return {std::move(controller)};
        } else {
            return util::NullOpt;
        }
    });
}
