#include "engine/entity/entity_type.h"

#include <utility>
#include "engine/phenyl_game.h"
#include "engine/game_object.h"
#include "component/component_serialiser.h"

using namespace game;

namespace game::detail {
    class ComponentFactory {
    private:
        ComponentSerialiser* serialiser;
        util::DataValue compData;
    public:
        ComponentFactory (ComponentSerialiser* serialiser, util::DataValue compData) : serialiser{serialiser}, compData{std::move(compData)} {}
        void addDefault (component::EntityView& entityView) const {
            if (!serialiser->hasComp(entityView)) {
                serialiser->deserialiseComp(entityView, compData);
            }
        }
    };
}

EntityType::EntityType (std::vector<component::EntityComponentFactory> factories) : factories{std::move(factories)} {}

void EntityType::addDefaultComponents (component::EntityView& entityView) const {
    for (const auto& i : factories) {
        i.addDefault(entityView);
    }
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
    std::vector<component::EntityComponentFactory> factories;
    for (const auto& [k, v] : obj.kv()) {
        /*auto* serialiser = gameObject.getSerialiser(k);
        if (serialiser) {
            factories.emplace_back(serialiser, v);
        }*/
        serialiser.makeFactory(k, v).ifPresent([&factories](component::EntityComponentFactory& factory) {
            factories.emplace_back(std::move(factory));
        });
    }

    return EntityType{std::move(factories)};
}