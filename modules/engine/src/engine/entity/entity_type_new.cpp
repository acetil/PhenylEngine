#include "engine/entity/entity_type_new.h"

#include <utility>
#include "engine/phenyl_game.h"
#include "engine/game_object.h"

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

EntityTypeNew::EntityTypeNew (std::vector<detail::ComponentFactory> factories) : factories{std::move(factories)} {}

void EntityTypeNew::addDefaultComponents (component::EntityView& entityView) const {
    for (const auto& i : factories) {
        i.addDefault(entityView);
    }
}

EntityTypeNew::EntityTypeNew (EntityTypeNew&&) noexcept = default;

EntityTypeNew& EntityTypeNew::operator= (EntityTypeNew&&) noexcept = default;

EntityTypeNew::EntityTypeNew () = default;

EntityTypeNew::~EntityTypeNew () = default;

EntityTypeNew game::makeEntityType (const util::DataValue& entityTypeData, detail::GameObject& gameObject) {
    if (!entityTypeData.is<util::DataObject>()) {
        logging::log(LEVEL_WARNING, "Invalid data for entity type!");
        return EntityTypeNew{{}};
    }

    const auto& obj = entityTypeData.get<util::DataObject>();
    std::vector<detail::ComponentFactory> factories;
    for (const auto& [k, v] : obj.kv()) {
        auto* serialiser = gameObject.getSerialiser(k);
        if (serialiser) {
            factories.emplace_back(serialiser, v);
        }
    }

    return EntityTypeNew{std::move(factories)};
}