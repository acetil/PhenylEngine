#include <stdexcept>

#include "engine/phenyl_game.h"
#include "graphics/phenyl_graphics.h"
#include "engine/game_object.h"
#include "engine/game_init.h"
#include "component/position.h"

using namespace game;

std::shared_ptr<game::detail::GameObject> PhenylGame::getShared () const {
    auto ptr = gameObject.lock();

#ifndef NDEBUG
    if (!ptr) {
        logging::log(LEVEL_FATAL, "Game object attempted to be accessed after it was deleted!");
        throw std::runtime_error("Game object attempted to be accessed after it was deleted!");
    }
#endif
    return ptr;
}


void PhenylGame::registerEntityType (const std::string& name, EntityTypeBuilder entityType) {
    getShared()->registerEntityType(name, entityType);
}

void PhenylGame::buildEntityTypes () {
    getShared()->buildEntityTypes();
}

component::EntityId PhenylGame::createNewEntityInstance (const std::string& name, float x, float y, float rot,
                                         const util::DataValue& data) {
    return getShared()->createNewEntityInstance(name, x, y, rot, data);
}

void PhenylGame::deleteEntityInstance (component::EntityId entityId) {
    getShared()->deleteEntityInstance(entityId);
}

void PhenylGame::registerTile (Tile* tile) {
    getShared()->registerTile(tile);
}

int PhenylGame::getTileId (const std::string& name) {
    return getShared()->getTileId(name);
}

Tile* PhenylGame::getTile (const std::string& name) {
    return getShared()->getTile(name);
}

Tile* PhenylGame::getTile (int tileId) {
    return getShared()->getTile(tileId);
}

void PhenylGame::updateEntityPosition () {
    getShared()->updateEntityPosition();
}

void PhenylGame::setTextureIds (graphics::TextureAtlas& atlas) {
    getShared()->setTextureIds(atlas);
}

void PhenylGame::setEntityComponentManager (component::EntityComponentManager::SharedPtr compManager) {
    getShared()->setEntityComponentManager(compManager);
}

void PhenylGame::updateEntitiesPrePhysics () {
    getShared()->updateEntitiesPrePhysics();
}

void PhenylGame::updateEntitiesPostPhysics () {
    getShared()->updateEntitiesPostPhysics();
}

event::EventBus::SharedPtr PhenylGame::getEventBus () {
    return getShared()->getEventBus();
}

std::shared_ptr<EntityController> PhenylGame::getController (const std::string& name) {
    return getShared()->getController(name);
}

void PhenylGame::reloadMap () {
    getShared()->reloadMap();
}

void PhenylGame::loadMap (Map::SharedPtr map) {
    getShared()->loadMap(map);
}

void PhenylGame::dumpMap (const std::string& filepath) {
    getShared()->dumpMap(filepath);
}

void PhenylGame::mapReloadRequest (event::ReloadMapEvent& event) {
    getShared()->mapReloadRequest(event);
}

void PhenylGame::mapDumpRequest (event::DumpMapEvent& event) {
    getShared()->mapDumpRequest(event);
}

void PhenylGame::mapLoadRequest (event::MapLoadRequestEvent& event) {
    getShared()->mapLoadRequest(event);
}

void PhenylGame::updateCamera (graphics::Camera& camera) {
    getShared()->updateCamera(camera);
}

GameCamera& PhenylGame::getCamera () {
    return getShared()->getCamera();
}

void PhenylGame::addEventHandlers (event::EventBus::SharedPtr eventBus) {
    getShared()->addEventHandlers(std::move(eventBus));
}

GameInput& PhenylGame::getGameInput () {
    return getShared()->getInput();
}

void PhenylGame::addComponentSerialiserInt (const std::string& component, std::unique_ptr<detail::ComponentSerialiser> serialiser) {
    getShared()->addComponentSerialiser(component, std::move(serialiser));
}

void PhenylGame::addDefaultSerialisers () {
    addComponentSerialiser<component::EntityMainComponent>("main_comp", [](const component::EntityMainComponent& comp) -> util::DataValue {
             return comp._serialise();
        }, [] (const util::DataValue& val) -> util::Optional<component::EntityMainComponent> {
            component::EntityMainComponent comp{};
            comp._deserialise(val);

            return {comp};
        });

    addComponentSerialiser<graphics::FixedModel>("fixed_model", [](graphics::FixedModel& comp) -> util::DataValue {
            return comp._serialise();
        }, [] (const util::DataValue& val) -> util::Optional<graphics::FixedModel> {
            graphics::FixedModel comp{};
            comp._deserialise(val);

            return {comp};
        });

    addComponentSerialiser<physics::CollisionComponent>("collision_comp", [](const physics::CollisionComponent& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<physics::CollisionComponent> {
        physics::CollisionComponent comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<graphics::AbsolutePosition>("absolute_position", [](const graphics::AbsolutePosition& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<graphics::AbsolutePosition> {
        graphics::AbsolutePosition comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<component::RotationComponent>("rotation", [](const component::RotationComponent& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<component::RotationComponent> {
        component::RotationComponent comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<EntityType>("entity_type", [](const EntityType& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<EntityType> {
        EntityType comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<component::Position2D>("pos_2D", component::serialisePos2D, component::deserialisePos2D);
}

PhenylGameHolder::~PhenylGameHolder () = default;

PhenylGame PhenylGameHolder::getGameObject () const {
    return PhenylGame(gameObject);
}

PhenylGameHolder::PhenylGameHolder () {
    gameObject = detail::GameObject::NewSharedPtr();
}

void PhenylGameHolder::initGame (const graphics::PhenylGraphics& graphics, event::EventBus::SharedPtr eventBus) {
    game::initGame(graphics, getGameObject(), eventBus);
}
