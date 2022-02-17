#include "engine/entity/entity_type.h"
#include "entity_type_functions.h"

#include "math.h"

using namespace game;

void game::setInitialEntityValues (const component::EntityComponentManager::SharedPtr& componentManager,
                                        EntityType& type, component::EntityId entityId, float x, float y, float rot) {
    //*componentManager->getObjectDataPtr<EntityType>(entityId).orElse(nullptr) = type;
    componentManager->addComponent<EntityType>(entityId, type);

    //auto physComp = componentManager->getObjectDataPtr<component::EntityMainComponent>(entityId).orElse(nullptr);

    component::EntityMainComponent physCompImpl{};

    physCompImpl.linFriction = type.defaultLinFriction;
    physCompImpl.constFriction = type.defaultConstFriction;
    physCompImpl.pos = {x, y};
    physCompImpl.vel = {0, 0};
    physCompImpl.acc = {0, 0};

    componentManager->addComponent<component::EntityMainComponent>(entityId, physCompImpl);

    //auto absPos = componentManager->getObjectDataPtr<graphics::AbsolutePosition>(entityId).orElse(nullptr);

    graphics::AbsolutePosition absPosImpl{};


    absPosImpl.transform = {{type.scale.x, 0}, {0, type.scale.y}};
    absPosImpl.pos = {x, y};


    //auto collComp = componentManager->getObjectDataPtr<physics::CollisionComponent>(entityId).orElse(nullptr);

    physics::CollisionComponent collCompImpl;

    collCompImpl.layers = type.defaultLayers;
    collCompImpl.resolveLayers = type.defaultResolveLayers;
    collCompImpl.eventLayer = type.defaultEventLayers;
    collCompImpl.masks = type.defaultCollisionMask;
    collCompImpl.bbMap = glm::mat2({{type.collisionScale.x, 0}, {0, type.collisionScale.y}});
    collCompImpl.mass = type.defaultMass;
    collCompImpl.outerRadius = sqrt(type.scale.x * type.scale.x + type.scale.y * type.scale.y);
    collCompImpl.pos = {x, y};

    //auto rotComp = componentManager->getObjectDataPtr<component::RotationComponent>(entityId).orElse(nullptr);

    component::RotationComponent rotCompImpl{};

    rotCompImpl = rot;
    //rotCompImpl.rotMatrix = {{cos(rot), sin(rot)}, {-sin(rot), cos(rot)}};

    collCompImpl.bbMap *= rotCompImpl.rotMatrix;
    absPosImpl.transform *= rotCompImpl.rotMatrix;


    componentManager->addComponent<graphics::AbsolutePosition>(entityId, absPosImpl);
    componentManager->addComponent<physics::CollisionComponent>(entityId, collCompImpl);
    componentManager->addComponent<component::RotationComponent>(entityId, rotCompImpl);

    //*componentManager->getObjectDataPtr<std::shared_ptr<EntityController>>(entityId).orElse(nullptr) = type.defaultController;
    componentManager->addComponent<std::shared_ptr<EntityController>>(entityId, type.defaultController);
    //*componentManager->getObjectDataPtr<AbstractEntity*>(entityId).orElse(nullptr) = type.entityFactory();
    componentManager->addComponent<AbstractEntity*>(entityId, type.entityFactory());

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
