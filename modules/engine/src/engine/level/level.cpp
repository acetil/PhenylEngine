#include "component/component.h"
#include "component/component_serializer.h"

#include "logging/logging.h"
#include "common/assets/asset.h"
#include "common/assets/assets.h"

#include "engine/level/level.h"

#include "common/serialization/backends.h"
#include "engine/level/level_manager.h"

using namespace phenyl::game;

static phenyl::Logger LOGGER{"LEVEL_MANAGER"};

class LevelEntitySerializable : public phenyl::common::ISerializable<phenyl::component::Entity> {
private:
    static constexpr std::string MEMBERS[] = {"components", "children", "prefab"};

    phenyl::component::EntityComponentSerializer& compSerializer;

    class ChildrenSerializable : public phenyl::common::ISerializable<phenyl::component::Entity> {
    private:
        LevelEntitySerializable& serializable;
    public:
        ChildrenSerializable (LevelEntitySerializable& serializable) : serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Entity::Children";
        }

        void serialize (phenyl::common::ISerializer& serializer, const phenyl::component::Entity& obj) override {
            auto& arrSerializer = serializer.serializeArr();

            for (auto i : obj.children()) {
                arrSerializer.serializeElement(serializable, i);
            }
        }

        void deserialize (phenyl::common::IDeserializer& deserializer, phenyl::component::Entity& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (phenyl::component::Entity& obj, phenyl::common::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                phenyl::component::Entity child;
                deserializer.next(serializable, child);
                obj.addChild(child);
            }
        }
    };
public:
    explicit LevelEntitySerializable (phenyl::component::EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}

    std::string_view name () const noexcept override {
        return "phenyl::Entity";
    }

    void serialize (phenyl::common::ISerializer& serializer, const phenyl::component::Entity& obj) override {
        auto compSerializable = compSerializer.entity();
        auto& objSerializer = serializer.serializeObj();

        objSerializer.serializeMember("components", compSerializable, obj);

    }

    void deserialize (phenyl::common::IDeserializer& deserializer, phenyl::component::Entity& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct(phenyl::component::Entity& obj, phenyl::common::IStructDeserializer& deserializer) override {
        auto entitySerializable = compSerializer.entity();
        if (!deserializer.next("components", entitySerializable, obj)) {
            throw phenyl::DeserializeException("Failed to deserialize entity components");
        }

        ChildrenSerializable serializable{*this};
        deserializer.next("children", serializable, obj);

        if (auto prefabOpt = deserializer.next<phenyl::common::Asset<phenyl::component::Prefab>>("prefab")) {
            (*prefabOpt)->instantiate(obj);
        }
    }
};

struct LevelMarker {};

class LevelSerializable : public phenyl::common::ISerializable<LevelMarker> {
private:
    static constexpr std::string MEMBERS[] = {"entities"};

    class EntitiesSerializable : public phenyl::common::ISerializable<LevelMarker> {
    private:
        phenyl::component::World& world;
        LevelEntitySerializable& serializable;
    public:
        EntitiesSerializable (phenyl::component::World& world, LevelEntitySerializable& serializable) : world{world}, serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Level::Entities";
        }

        void serialize (phenyl::common::ISerializer& serializer, const LevelMarker& obj) override {
            auto& arrSerializer = serializer.serializeArr();
            for (auto entity : world.root()) {
                arrSerializer.serializeElement(serializable, entity);
            }
        }

        void deserialize (phenyl::common::IDeserializer& deserializer, LevelMarker& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (LevelMarker& obj, phenyl::common::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                auto entity = world.create();
                deserializer.next(serializable, entity);
            }
        }
    };
    LevelEntitySerializable entitySerializable;
    EntitiesSerializable entitiesSerializable;
public:
    LevelSerializable (phenyl::component::World& world, phenyl::component::EntityComponentSerializer& compSerializer) : entitySerializable{compSerializer}, entitiesSerializable{world, entitySerializable} {}

    std::string_view name () const noexcept override {
        return "phenyl::Level";
    }

    void serialize (phenyl::common::ISerializer& serializer, const LevelMarker& obj) override {
        auto& objSerializer = serializer.serializeObj();
        objSerializer.serializeMember("entities", entitiesSerializable, obj);
    }

    void deserialize (phenyl::common::IDeserializer& deserializer, LevelMarker& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct (LevelMarker& obj, phenyl::common::IStructDeserializer& deserializer) override {
        if (!deserializer.next("entities", entitiesSerializable, obj)) {
            throw phenyl::DeserializeException("Failed to deserialize entities in level!");
        }
    }
};

LevelManager::LevelManager (component::World& world, component::EntityComponentSerializer& serializer) : world{world}, serializer{serializer} {}
LevelManager::~LevelManager () = default;

Level* LevelManager::load (std::ifstream& data, std::size_t id) {
    levels[id] = std::make_unique<Level>(Level{std::move(data), world, serializer});

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
    PHENYL_LOGI(LOGGER, "Dumping level");
    LevelSerializable serializable{world, serializer};
    LevelMarker marker{};
    common::SerializeToJson(file, serializable, marker, true);
}

Level* LevelManager::load (Level&& obj, std::size_t id) {
    PHENYL_LOGD(LOGGER, "Loading already created level");
    levels[id] = std::make_unique<Level>(std::move(obj));
    return levels[id].get();
}

std::string_view LevelManager::getName () const noexcept {
    return "LevelManager";
}

Level::Level (std::ifstream file, component::World& world, component::EntityComponentSerializer& serializer) : file{std::move(file)}, world{world}, serializer{serializer} {
    PHENYL_DASSERT(!this->file.bad());
    startPos = this->file.tellg();
}

void Level::load (bool additive) {
    if (!additive) {
        PHENYL_LOGD(LOGGER, "Clearing entities due to level load");
        world.clear();
    }

    world.deferSignals();

    file.seekg(startPos);
    LevelSerializable serializable{world, serializer};
    LevelMarker marker{};
    common::DeserializeFromJson(file, serializable, marker);

    world.deferSignalsEnd();
}