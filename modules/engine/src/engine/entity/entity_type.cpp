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

EntityType game::makeEntityType (const util::DataValue& entityTypeData, component::EntitySerialiser& serialiser) {
    if (!entityTypeData.is<util::DataObject>()) {
        logging::log(LEVEL_WARNING, "Invalid data for entity type!");
        return EntityType{{}};
    }

    const auto& obj = entityTypeData.get<util::DataObject>();
    /*std::vector<component::EntityComponentFactory> factories;
    for (const auto& [k, v] : obj.kv()) {
        serialiser.makeFactory(k, v).ifPresent([&factories](component::EntityComponentFactory& factory) {
            factories.emplace_back(std::move(factory));
        });
    }*/

    //return EntityType{std::move(factories)};
    return EntityType{std::vector<component::EntityComponentFactory>{}};
}