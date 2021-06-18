#include "entity_type.h"
#include "entity_type_functions.h"

#include "math.h"

using namespace game;

void game::setInitialEntityValues (const component::EntityComponentManager::SharedPtr& componentManager,
                                        EntityType& type, int entityId, float x, float y, float rot) {
    *componentManager->getObjectDataPtr<EntityType>(entityId) = type;

    auto physComp = componentManager->getObjectDataPtr<component::EntityMainComponent>(entityId);
    physComp->linFriction = type.defaultLinFriction;
    physComp->constFriction = type.defaultConstFriction;
    physComp->pos = {x, y};
    physComp->vel = {0, 0};
    physComp->acc = {0, 0};
    auto absPos = componentManager->getObjectDataPtr<graphics::AbsolutePosition>(entityId);
    absPos->transform = {{type.scale.x, 0}, {0, type.scale.y}};
    absPos->pos = {x, y};

    auto collComp = componentManager->getObjectDataPtr<physics::CollisionComponent>(entityId);
    collComp->layers = type.defaultLayers;
    collComp->resolveLayers = type.defaultResolveLayers;
    collComp->eventLayer = type.defaultEventLayers;
    collComp->masks = type.defaultCollisionMask;
    collComp->bbMap = glm::mat2({{type.collisionScale.x, 0}, {0, type.collisionScale.y}});
    collComp->mass = type.defaultMass;
    collComp->outerRadius = sqrt(type.scale.x * type.scale.x + type.scale.y * type.scale.y);
    collComp->pos = {x, y};

    auto rotComp = componentManager->getObjectDataPtr<component::RotationComponent>(entityId);
    rotComp->rotation = rot;
    rotComp->rotMatrix = {{cos(rot), -sin(rot)}, {sin(rot), cos(rot)}};

    collComp->bbMap *= rotComp->rotMatrix;
    absPos->transform *= rotComp->rotMatrix;

    *componentManager->getObjectDataPtr<std::shared_ptr<EntityController>>(entityId) = type.defaultController;
    *componentManager->getObjectDataPtr<AbstractEntity*>(entityId) = type.entityFactory();

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
