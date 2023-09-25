#include <nlohmann/json.hpp>

#include "component/component.h"
#include "component/component_serializer.h"

#include "logging/logging.h"
#include "common/assets/asset.h"
#include "common/assets/assets.h"

#include "engine/level/level.h"
#include "level_manager.h"

using namespace phenyl::game;

namespace phenyl::game::detail {
    struct LevelEntity {
        nlohmann::json components;
        common::Asset<component::Prefab> prefab;
        std::size_t numChildren;
    };
}

namespace {
    std::size_t parseEntity (const nlohmann::json& json, std::vector<detail::LevelEntity>& entities);
    phenyl::component::Prefab::Instantiator getInstantatior (const phenyl::common::Asset<phenyl::component::Prefab>& prefab, phenyl::component::ComponentManager* manager);
}

LevelManager::LevelManager (component::ComponentManager* manager, component::EntitySerializer* serializer) : manager{manager}, serializer{serializer} {}
LevelManager::~LevelManager () = default;

Level* LevelManager::load (std::istream& data, std::size_t id) {
    assert(!levels.contains(id));
    nlohmann::json json;
    data >> json;

    if (!json.contains("entities")) {
        logging::log(LEVEL_ERROR, "Failed to find entities field in level!");
        return nullptr;
    }

    auto& entitiesJson = json.at("entities");
    if (!entitiesJson.is_array()) {
        logging::log(LEVEL_ERROR, "Expected array for entities, got {}!", entitiesJson.type_name());
        return nullptr;
    }

    std::vector<detail::LevelEntity> entities;
    entities.reserve(entitiesJson.size());

    for (const auto& i : entitiesJson.get<nlohmann::json::array_t>()) {
        if (!parseEntity(i, entities)) {
            logging::log(LEVEL_ERROR, "Failed to parse entity!");
            return nullptr;
        }
    }

    levels[id] = std::make_unique<Level>(Level{manager, serializer, std::move(entities)});

    return levels[id].get();
}

void LevelManager::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        levels.remove(id);
    }
}

const char* LevelManager::getFileType () const {
    return ".json";
}

void LevelManager::selfRegister () {
    common::Assets::AddManager(this);
}

void LevelManager::dump (std::ostream& file) const {
    auto entities = nlohmann::json::array_t{};
    for (auto i : manager->root()) {
        entities.emplace_back(dumpEntity(i));
    }

    nlohmann::json level;
    level["entities"] = std::move(entities);
    file << std::setw(4) << level << "\n";
}

nlohmann::json LevelManager::dumpEntity (component::Entity entity) const {
    nlohmann::json json;
    json["components"] = serializer->serializeEntity(entity);

    nlohmann::json::array_t children;
    for (auto i : entity.children()) {
        children.emplace_back(dumpEntity(i));
    }

    json["children"] = std::move(children);

    return json;
}

Level* LevelManager::load (Level&& obj, std::size_t id) {
    levels[id] = std::make_unique<Level>(std::move(obj));
    return levels[id].get();
}

Level::Level (component::ComponentManager* manager, component::EntitySerializer* serializer, std::vector<detail::LevelEntity> entities) : manager{manager}, serializer{serializer}, entities{std::move(entities)} {}

void Level::load (bool additive) {
    if (!additive) {
        manager->clear();
    }

    manager->deferSignals();
    std::size_t index = 0;
    while (index < entities.size()) {
        loadEntity(index);
        index += entities[index].numChildren + 1;
    }

    manager->deferSignalsEnd();
}

phenyl::component::Entity Level::loadEntity (std::size_t index) {
    assert(index < entities.size());
    const auto& entity = entities[index];

    auto instantatior = getInstantatior(entity.prefab, manager);
    serializer->deserializeEntity(instantatior, entity.components);
    auto childIndex = index + 1;
    while (childIndex < index + entity.numChildren + 1) {
        instantatior.withChild(loadEntity(childIndex));
        childIndex += entities[childIndex].numChildren;
    }

    return instantatior.complete();
}

namespace {
    std::size_t parseEntity (const nlohmann::json& json, std::vector<detail::LevelEntity>& entities) {
        if (!json.is_object()) {
            logging::log(LEVEL_ERROR, "Expected object for entity, got {}!", json.type_name());
            return 0;
        }

        if (!json.contains("components")) {
            logging::log(LEVEL_ERROR, "Failed to find components field in entity!");
            return 0;
        }

        phenyl::common::Asset<phenyl::component::Prefab> prefab;
        if (json.contains("prefab")) {
            if (!json.at("prefab").is_string()) {
                logging::log(LEVEL_ERROR, "Expected string for prefab field, got {}!", json.at("prefab").type_name());
                return 0;
            }

            prefab = phenyl::common::Assets::Load<phenyl::component::Prefab>(json.at("prefab").get<std::string>());
            if (!prefab) {
                logging::log(LEVEL_ERROR, "Failed to load prefab {} for entity!", json.at("prefab").get<std::string>());
            }
        }

        entities.emplace_back(json.at("components"), std::move(prefab), 0);
        auto& entity = entities.back();

        if (json.contains("children")) {
            if (!json.at("children").is_array()) {
                logging::log(LEVEL_ERROR, "Expected array for children field, got {}!", json.at("children").type_name());
                return 0;
            }

            for (auto& i : json.at("children").get<nlohmann::json::array_t>()) {
                auto n = parseEntity(i, entities);
                if (!n) {
                    return 0;
                }

                entity.numChildren += n;
            }
        }

        return entity.numChildren + 1;
    }

    phenyl::component::Prefab::Instantiator getInstantatior (const phenyl::common::Asset<phenyl::component::Prefab>& prefab, phenyl::component::ComponentManager* manager) {
        if (prefab) {
            return prefab->instantiate();
        } else {
            return phenyl::component::Prefab::NullPrefab(manager).instantiate();
        }
    }
}