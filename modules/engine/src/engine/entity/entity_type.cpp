#include "engine/entity/entity_type.h"
#include "entity_type_functions.h"

#include "math.h"
#include "component/position.h"

using namespace game;

void game::setInitialEntityValues (const component::EntityComponentManager::SharedPtr& componentManager,
                                        EntityType& type, component::EntityId entityId, float x, float y, float rot) {
    //*componentManager->getObjectDataPtr<EntityType>(entityId).orElse(nullptr) = type;
    componentManager->addComponent<EntityType>(entityId, type);

    //auto physComp = componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(entityId).orElse(nullptr);

    component::FrictionKinematicsMotion2D physCompImpl{};

    physCompImpl.linFriction = type.defaultLinFriction;
    physCompImpl.constFriction = type.defaultConstFriction;
    //physCompImpl.pos = {x, y};
    physCompImpl.velocity = {0, 0};
    physCompImpl.acceleration = {0, 0};

    componentManager->addComponent<component::FrictionKinematicsMotion2D>(entityId, physCompImpl);

    //auto absPos = componentManager->getObjectDataPtr<graphics::Transform2D>(entityId).orElse(nullptr);

    graphics::Transform2D absPosImpl{};


    absPosImpl.transform = {{type.scale.x, 0}, {0, type.scale.y}};
    absPosImpl.rotTransform = absPosImpl.transform;
    //absPosImpl.pos = {x, y};


    //auto collComp = componentManager->getObjectDataPtr<physics::CollisionComponent2D>(entityId).orElse(nullptr);

    physics::CollisionComponent2D collCompImpl;

    collCompImpl.layers = type.defaultLayers;
    collCompImpl.resolveLayers = type.defaultResolveLayers;
    collCompImpl.eventLayer = type.defaultEventLayers;
    collCompImpl.masks = type.defaultCollisionMask;
    collCompImpl.bbMap = glm::mat2({{type.collisionScale.x, 0}, {0, type.collisionScale.y}});
    collCompImpl.rotBBMap = collCompImpl.bbMap;
    collCompImpl.mass = type.defaultMass;
    collCompImpl.outerRadius = sqrt(type.scale.x * type.scale.x + type.scale.y * type.scale.y);
    //collCompImpl.pos = {x, y};

    //auto rotComp = componentManager->getObjectDataPtr<component::Rotation2D>(entityId).orElse(nullptr);

    component::Rotation2D rotCompImpl{};

    rotCompImpl = rot;
    //rotCompImpl.rotMatrix = {{cos(rot), sin(rot)}, {-sin(rot), cos(rot)}};

    //collCompImpl.bbMap *= rotCompImpl.rotMatrix;
    //absPosImpl.transform *= rotCompImpl.rotMatrix;


    componentManager->addComponent<graphics::Transform2D>(entityId, absPosImpl);
    componentManager->addComponent<physics::CollisionComponent2D>(entityId, collCompImpl);
    componentManager->addComponent<component::Rotation2D>(entityId, rotCompImpl);

    //*componentManager->getObjectDataPtr<std::shared_ptr<EntityController>>(entityId).orElse(nullptr) = type.defaultController;
    componentManager->addComponent<std::shared_ptr<EntityController>>(entityId, type.defaultController);
    //*componentManager->getObjectDataPtr<AbstractEntity*>(entityId).orElse(nullptr) = type.entityFactory();
    componentManager->addComponent<AbstractEntity*>(entityId, type.entityFactory());
    componentManager->addComponent<component::Position2D>(entityId, glm::vec2{x, y});

}

EntityTypeBuilder& EntityTypeBuilder::setScale (float _scale) {
    scale = {_scale, _scale};
    if (!hasSetCollScale) {
        collScale = scale;
    }
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::setScale (float xScale, float yScale) {
    scale = {xScale, yScale};
    if (!hasSetCollScale) {
        collScale = scale;
    }
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::setConstFriction (float friction) {
    defaultConstFriction = friction;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::setLinearFriction (float friction) {
    defaultLinFriction = friction;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::setMass (float mass) {
    defaultMass = mass;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::addLayers (unsigned int layers) {
    defaultLayers |= layers;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::addCollisionLayers (unsigned int layers) {
    defaultCollisionMask |= layers;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::addEventLayers (unsigned int layers) {
    defaultEventLayers |= layers;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::addResolveLayers (unsigned int layers) {
    defaultResolveLayers |= layers;
    return *this;
}

EntityType EntityTypeBuilder::build (const std::unordered_map<std::string, std::shared_ptr<EntityController>>& controllerMap) {
    auto type = EntityType();

    type.entityFactory = entityFactory;

    type.scale = scale;

    type.defaultConstFriction = defaultConstFriction;
    type.defaultLinFriction = defaultLinFriction;

    type.defaultMass = defaultMass;

    type.defaultLayers = defaultLayers;
    type.defaultCollisionMask = defaultCollisionMask;
    type.defaultEventLayers = defaultEventLayers;
    type.defaultResolveLayers = defaultResolveLayers;

    type.collisionScale = collScale;

    type.defaultController = controllerMap.at(controller);

    type.controller = controller;

    type.typeName = typeName;

    return type;
}

EntityTypeBuilder& EntityTypeBuilder::setCollisonScale (float _scale) {
    collScale = {_scale, _scale};
    hasSetCollScale = true;
    return *this;
}

EntityTypeBuilder& EntityTypeBuilder::setCollisonScale (float xScale, float yScale) {
    collScale = {xScale, yScale};
    hasSetCollScale = true;
    return *this;
}
