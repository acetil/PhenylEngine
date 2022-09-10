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
    addComponentSerialiser<component::FrictionKinematicsMotion2D>("friction_kinematics_2D", [](const component::FrictionKinematicsMotion2D& comp) -> util::DataValue {
             return comp._serialise();
        }, [] (const util::DataValue& val) -> util::Optional<component::FrictionKinematicsMotion2D> {
            component::FrictionKinematicsMotion2D comp{};
            comp._deserialise(val);

            return {comp};
        });

    addComponentSerialiser<graphics::Model2D>("model_2D", [](graphics::Model2D& comp) -> util::DataValue {
            return comp._serialise();
        }, [] (const util::DataValue& val) -> util::Optional<graphics::Model2D> {
            graphics::Model2D comp{};
            comp._deserialise(val);

            return {comp};
        });

    addComponentSerialiser<physics::CollisionComponent2D>("collision_comp_2D", [](const physics::CollisionComponent2D& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<physics::CollisionComponent2D> {
        physics::CollisionComponent2D comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<graphics::Transform2D>("transform_2D", [](const graphics::Transform2D& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<graphics::Transform2D> {
        graphics::Transform2D comp{};
        comp._deserialise(val);

        return {comp};
    });

    addComponentSerialiser<component::Rotation2D>("rotation_2D", [](const component::Rotation2D& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<component::Rotation2D> {
        component::Rotation2D comp{};
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

    addComponentSerialiser<EntityTypeComponent>("type", [](const EntityTypeComponent& comp) -> util::DataValue {
        return (util::DataValue)comp.typeId;
    }, [](const util::DataValue& val) -> util::Optional<EntityTypeComponent> {
        if (val.is<std::string>()) {
            return {EntityTypeComponent{val.get<std::string>()}};
        } else {
            return util::NullOpt;
        }
    });

    addComponentSerialiser<std::shared_ptr<EntityController>>("controller", [] (const std::shared_ptr<EntityController>& comp) -> util::DataValue {
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

void PhenylGame::addEntityType (const std::string& typeId, const std::string& filepath) {
    getShared()->addEntityType(typeId, filepath);
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
