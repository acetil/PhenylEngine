#include "core/world.h"
#include "core/serialization/component_serializer.h"

#include "logging/logging.h"
#include "core/assets/asset.h"
#include "core/assets/assets.h"

#include "engine/level/level.h"

#include "core/serialization/backends.h"
#include "engine/level/level_manager.h"

using namespace phenyl::game;

static phenyl::Logger LOGGER{"LEVEL_MANAGER"};

class LevelEntitySerializable : public phenyl::core::ISerializable<phenyl::core::Entity> {
private:
    static constexpr std::string MEMBERS[] = {"components", "children", "prefab"};

    phenyl::core::EntityComponentSerializer& compSerializer;

    class ChildrenSerializable : public phenyl::core::ISerializable<phenyl::core::Entity> {
    private:
        LevelEntitySerializable& serializable;
    public:
        ChildrenSerializable (LevelEntitySerializable& serializable) : serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Entity::Children";
        }

        void serialize (phenyl::core::ISerializer& serializer, const phenyl::core::Entity& obj) override {
            auto& arrSerializer = serializer.serializeArr();

            for (auto i : obj.children()) {
                arrSerializer.serializeElement(serializable, i);
            }
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, phenyl::core::Entity& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (phenyl::core::Entity& obj, phenyl::core::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                phenyl::core::Entity child;
                deserializer.next(serializable, child);
                obj.addChild(child);
            }
        }
    };
public:
    explicit LevelEntitySerializable (phenyl::core::EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}

    std::string_view name () const noexcept override {
        return "phenyl::Entity";
    }

    void serialize (phenyl::core::ISerializer& serializer, const phenyl::core::Entity& obj) override {
        auto compSerializable = compSerializer.entity();
        auto& objSerializer = serializer.serializeObj();

        objSerializer.serializeMember("components", compSerializable, obj);

    }

    void deserialize (phenyl::core::IDeserializer& deserializer, phenyl::core::Entity& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct(phenyl::core::Entity& obj, phenyl::core::IStructDeserializer& deserializer) override {
        auto entitySerializable = compSerializer.entity();
        if (!deserializer.next("components", entitySerializable, obj)) {
            throw phenyl::DeserializeException("Failed to deserialize entity components");
        }

        ChildrenSerializable serializable{*this};
        deserializer.next("children", serializable, obj);

        if (auto prefabOpt = deserializer.next<phenyl::core::Asset<phenyl::core::Prefab>>("prefab")) {
            (*prefabOpt)->instantiate(obj);
        }
    }
};

struct LevelMarker {};

class LevelSerializable : public phenyl::core::ISerializable<LevelMarker> {
private:
    static constexpr std::string MEMBERS[] = {"entities"};

    class EntitiesSerializable : public phenyl::core::ISerializable<LevelMarker> {
    private:
        phenyl::core::World& world;
        LevelEntitySerializable& serializable;
    public:
        EntitiesSerializable (phenyl::core::World& world, LevelEntitySerializable& serializable) : world{world}, serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Level::Entities";
        }

        void serialize (phenyl::core::ISerializer& serializer, const LevelMarker& obj) override {
            auto& arrSerializer = serializer.serializeArr();
            for (auto entity : world.root()) {
                arrSerializer.serializeElement(serializable, entity);
            }
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, LevelMarker& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (LevelMarker& obj, phenyl::core::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                auto entity = world.create();
                deserializer.next(serializable, entity);
            }
        }
    };
    LevelEntitySerializable entitySerializable;
    EntitiesSerializable entitiesSerializable;
public:
    LevelSerializable (phenyl::core::World& world, phenyl::core::EntityComponentSerializer& compSerializer) : entitySerializable{compSerializer}, entitiesSerializable{world, entitySerializable} {}

    std::string_view name () const noexcept override {
        return "phenyl::Level";
    }

    void serialize (phenyl::core::ISerializer& serializer, const LevelMarker& obj) override {
        auto& objSerializer = serializer.serializeObj();
        objSerializer.serializeMember("entities", entitiesSerializable, obj);
    }

    void deserialize (phenyl::core::IDeserializer& deserializer, LevelMarker& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct (LevelMarker& obj, phenyl::core::IStructDeserializer& deserializer) override {
        if (!deserializer.next("entities", entitiesSerializable, obj)) {
            throw phenyl::DeserializeException("Failed to deserialize entities in level!");
        }
    }
};

LevelManager::LevelManager (core::World& world, core::EntityComponentSerializer& serializer) : world{world}, serializer{serializer} {}
LevelManager::~LevelManager () = default;

Level* LevelManager::load (std::ifstream& data, std::size_t id) {
    levels[id] = std::make_unique<Level>(Level{std::move(data), *this});

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
    core::Assets::AddManager(this);
}

void LevelManager::queueLoad (core::Asset<Level> level, bool additive) {
    if (!additive) {
        PHENYL_LOGI_IF((!queuedLoads.empty()), LOGGER, "Dropping {} queued loads because of non-additive load", queuedLoads.size());
        queuedLoads.clear();
        queuedClear = true;
    }

    queuedLoads.emplace_back(std::move(level));
}

void LevelManager::loadLevels () {
    if (queuedClear) {
        PHENYL_LOGD(LOGGER, "Clearing entities due to level load");
        world.clear();
        queuedClear = false;
    }

    if (queuedLoads.empty()) {
        return;
    }

    PHENYL_LOGD(LOGGER, "Loading {} queued levels", queuedLoads.size());
    world.deferSignals();
    for (auto& i : queuedLoads) {
        i->loadImmediate(world, serializer);
    }
    queuedLoads.clear();
    world.deferSignalsEnd();
}

void LevelManager::dump (std::ostream& file) const {
    PHENYL_LOGI(LOGGER, "Dumping level");
    LevelSerializable serializable{world, serializer};
    LevelMarker marker{};
    core::SerializeToJson(file, serializable, marker, true);
}

Level* LevelManager::load (Level&& obj, std::size_t id) {
    PHENYL_LOGD(LOGGER, "Loading already created level");
    levels[id] = std::make_unique<Level>(std::move(obj));
    return levels[id].get();
}

std::string_view LevelManager::getName () const noexcept {
    return "LevelManager";
}

Level::Level (std::ifstream file, LevelManager& manager) : file{std::move(file)}, manager{manager} {
    PHENYL_DASSERT(!this->file.bad());
    startPos = this->file.tellg();
}

void Level::loadImmediate (core::World& world, core::EntityComponentSerializer& serializer) {
    file.seekg(startPos);
    LevelSerializable serializable{world, serializer};
    LevelMarker marker{};
    core::DeserializeFromJson(file, serializable, marker);
}

void Level::load (bool additive) {
    // if (!additive) {
    //     PHENYL_LOGD(LOGGER, "Clearing entities due to level load");
    //     world.clear();
    // }
    //
    // world.deferSignals();
    //
    // file.seekg(startPos);
    // LevelSerializable serializable{world, serializer};
    // LevelMarker marker{};
    // common::DeserializeFromJson(file, serializable, marker);
    //
    // world.deferSignalsEnd();
    manager.queueLoad(assetFromThis(), additive);
}
