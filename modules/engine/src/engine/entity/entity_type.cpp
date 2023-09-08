#include "engine/entity/entity_type.h"

#include <utility>
#include "component/component_serialiser.h"

using namespace game;

EntityType::EntityType (std::vector<component::EntityComponentFactory> factories) : factories{std::move(factories)} {}

void EntityType::addDefaultComponents (component::Entity& entityView) const {
    /*for (const auto& i : factories) {
        i.addDefault(entityView);
    }*/
}

EntityType::EntityType (EntityType&&) noexcept = default;

EntityType& EntityType::operator= (EntityType&&) noexcept = default;

EntityType::EntityType () = default;

EntityType::~EntityType () = default;
