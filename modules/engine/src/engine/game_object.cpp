#include <string>
#include <utility>
#include <math.h>

#include "engine/game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "common/events/entity_creation.h"
#include "engine/entity/controller/entity_controller.h"
#include "entity/entity_type_functions.h"
#include "common/events/map_load.h"
#include "component/component_serialisation.h"
#include "engine/map/map_reader.h"

using namespace game;

detail::GameObject::~GameObject () {
    for (auto const& x : entityRegistry) {
        delete x.second;
    }
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
component::EntityId detail::GameObject::createNewEntityInstance (const std::string& name, float x, float y, float rot, const util::DataValue& data) {
    // TODO: requires refactor
    if (entityTypes.count(name) == 0) {
        logging::log(LEVEL_WARNING, "Attempted creation of entity with name '{}' which doesn't exist!", name);
        return {0, 0};
    } else {
        auto entityId = entityComponentManager->createEntity();
        setInitialEntityValues(entityComponentManager, entityTypes[name], entityId, x, y, rot);
        //auto viewCore = view::ViewCore(entityComponentManager);
        auto entityView = entityComponentManager->getEntityView(entityId);
        auto gameView = view::GameView(this);
        entityView.getComponent<std::shared_ptr<EntityController>>().getUnsafe()->initEntity(entityView, gameView, data);
        eventBus->raise(event::EntityCreationEvent(x, y, 0.1f, entityComponentManager,
                                                   entityComponentManager->getObjectData<AbstractEntity*>(
                                                           entityId).orElse(nullptr), entityId,
                                                   entityView, gameView));
        return entityId;
    }
}

component::EntityId detail::GameObject::deserialiseEntity (const std::string& type, float x, float y, float rot, const util::DataValue& serialised) {
    auto entityId = entityComponentManager->createEntity();
    auto serialisedObj = serialised.get<util::DataObject>();
    setInitialEntityValues(entityComponentManager, entityTypes.at(type), entityId, x, y, rot);

    component::deserialiseComps<ENTITY_LIST>(entityComponentManager, entityId, serialisedObj);

    //auto viewCore = view::ViewCore(entityComponentManager);
    //auto entityView = view::EntityView(viewCore, entityId, eventBus);
    auto entityView = entityComponentManager->getEntityView(entityId);
    auto gameView = view::GameView(this);

    //entityView.

    entityView.getComponent<std::shared_ptr<EntityController>>().ifPresent([&entityView, &gameView, &serialisedObj](std::shared_ptr<EntityController>& controller){
        controller->initEntity(entityView, gameView, serialisedObj["data"]);
    });
    eventBus->raise(event::EntityCreationEvent(x, y, 0.1f, entityComponentManager,
                                               entityComponentManager->getObjectData<AbstractEntity*>(entityId).orElse(
                                                       nullptr), entityId,
                                               entityView, gameView));
    return entityId;
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
void detail::GameObject::setTextureIds (graphics::TextureAtlas& atlas) {
    for (auto const& it : controllers) {
        it.second->setTextureIds(atlas);
    }
}
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

void detail::GameObject::registerEntityType (const std::string& name, EntityTypeBuilder entityTypeBuilder) {
    entityTypeBuilders[name] = std::move(entityTypeBuilder);
}

void detail::GameObject::buildEntityTypes () {
    for (auto [name, builder] : entityTypeBuilders) {
        entityTypes[name] = builder.build(controllers);
    }
}

std::shared_ptr<EntityController> detail::GameObject::getController (const std::string& name) {
    return controllers[name];
}

void detail::GameObject::loadMap (Map::SharedPtr map) {
    entityComponentManager->clear();
    this->gameMap = std::move(map);

    for (auto& i : gameMap->getEntities()) {
        //createNewEntityInstance(i.entityType, i.x, i.y, i.rotation, i.data);
        deserialiseEntity(i.entityType, i.x, i.y, i.rotation, i.data);
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
        auto compData = component::serialise<ENTITY_LIST>(entityComponentManager, ids[i]);
        //auto entityView = view::EntityView(view::ViewCore(entityComponentManager), ids[i], eventBus);
        auto entityView = entityComponentManager->getEntityView(ids[i]);
        compData.get<util::DataObject>()["data"] =
                entityComponentManager->getObjectData<std::shared_ptr<game::EntityController>>(ids[i]).orElse(nullptr)->getData(entityView, gameView);
        compData.get<util::DataObject>()["type"] = entityComponentManager->getObjectData<game::EntityType>(ids[i]).orElse(game::EntityType()).typeName;
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
