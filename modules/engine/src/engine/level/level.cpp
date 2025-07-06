#include "engine/level/level.h"

#include "core/assets/asset.h"
#include "core/assets/assets.h"
#include "core/serialization/backends.h"
#include "core/serialization/component_serializer.h"
#include "core/world.h"
#include "engine/level/level_manager.h"
#include "logging/logging.h"

using namespace phenyl::game;

static phenyl::Logger LOGGER{"LEVEL_MANAGER"};

class LevelEntitySerializable : public phenyl::core::ISerializable<phenyl::core::Entity> {
private:
    static constexpr std::string MEMBERS[] = {"components", "children", "prefab"};

    phenyl::core::EntityComponentSerializer& m_serializer;

    class ChildrenSerializable : public phenyl::core::ISerializable<phenyl::core::Entity> {
    private:
        LevelEntitySerializable& m_serializable;

    public:
        ChildrenSerializable (LevelEntitySerializable& serializable) : m_serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Entity::Children";
        }

        void serialize (phenyl::core::ISerializer& serializer, const phenyl::core::Entity& obj) override {
            auto& arrSerializer = serializer.serializeArr();

            for (auto i : obj.children()) {
                arrSerializer.serializeElement(m_serializable, i);
            }
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, phenyl::core::Entity& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (phenyl::core::Entity& obj, phenyl::core::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                phenyl::core::Entity child;
                deserializer.next(m_serializable, child);
                obj.addChild(child);
            }
        }
    };

public:
    explicit LevelEntitySerializable (phenyl::core::EntityComponentSerializer& compSerializer) :
        m_serializer{compSerializer} {}

    std::string_view name () const noexcept override {
        return "phenyl::Entity";
    }

    void serialize (phenyl::core::ISerializer& serializer, const phenyl::core::Entity& obj) override {
        auto compSerializable = m_serializer.entity();
        auto& objSerializer = serializer.serializeObj();

        objSerializer.serializeMember("components", compSerializable, obj);
    }

    void deserialize (phenyl::core::IDeserializer& deserializer, phenyl::core::Entity& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct (phenyl::core::Entity& obj, phenyl::core::IStructDeserializer& deserializer) override {
        auto entitySerializable = m_serializer.entity();
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
        phenyl::core::World& m_world;
        LevelEntitySerializable& m_serializable;

    public:
        EntitiesSerializable (phenyl::core::World& world, LevelEntitySerializable& serializable) :
            m_world{world},
            m_serializable{serializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Level::Entities";
        }

        void serialize (phenyl::core::ISerializer& serializer, const LevelMarker& obj) override {
            auto& arrSerializer = serializer.serializeArr();
            for (auto entity : m_world.root()) {
                arrSerializer.serializeElement(m_serializable, entity);
            }
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, LevelMarker& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (LevelMarker& obj, phenyl::core::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                auto entity = m_world.create();
                deserializer.next(m_serializable, entity);
            }
        }
    };

    LevelEntitySerializable m_entitySerializable;
    EntitiesSerializable m_entitiesSerializable;

public:
    LevelSerializable (phenyl::core::World& world, phenyl::core::EntityComponentSerializer& compSerializer) :
        m_entitySerializable{compSerializer},
        m_entitiesSerializable{world, m_entitySerializable} {}

    std::string_view name () const noexcept override {
        return "phenyl::Level";
    }

    void serialize (phenyl::core::ISerializer& serializer, const LevelMarker& obj) override {
        auto& objSerializer = serializer.serializeObj();
        objSerializer.serializeMember("entities", m_entitiesSerializable, obj);
    }

    void deserialize (phenyl::core::IDeserializer& deserializer, LevelMarker& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct (LevelMarker& obj, phenyl::core::IStructDeserializer& deserializer) override {
        if (!deserializer.next("entities", m_entitiesSerializable, obj)) {
            throw phenyl::DeserializeException("Failed to deserialize entities in level!");
        }
    }
};

LevelManager::LevelManager (core::World& world, core::EntityComponentSerializer& serializer) :
    m_world{world},
    m_serializer{serializer} {}

LevelManager::~LevelManager () = default;

Level* LevelManager::load (std::ifstream& data, std::size_t id) {
    m_levels[id] = std::make_unique<Level>(Level{std::move(data), *this});

    return m_levels[id].get();
}

std::shared_ptr<Level> LevelManager::load2 (std::ifstream& data) {
    return std::make_shared<Level>(Level{std::move(data), *this});
}

void LevelManager::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        m_levels.erase(id);
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
        PHENYL_LOGI_IF((!m_queuedLoads.empty()), LOGGER, "Dropping {} queued loads because of non-additive load",
            m_queuedLoads.size());
        m_queuedLoads.clear();
        m_queuedClear = true;
    }

    m_queuedLoads.emplace_back(std::move(level));
}

void LevelManager::loadLevels () {
    if (m_queuedClear) {
        PHENYL_LOGD(LOGGER, "Clearing entities due to level load");
        m_world.clear();
        m_queuedClear = false;
    }

    if (m_queuedLoads.empty()) {
        return;
    }

    PHENYL_LOGD(LOGGER, "Loading {} queued levels", m_queuedLoads.size());
    m_world.deferSignals();
    for (auto& i : m_queuedLoads) {
        i->loadImmediate(m_world, m_serializer);
    }
    m_queuedLoads.clear();
    m_world.deferSignalsEnd();
}

void LevelManager::dump (std::ostream& file) const {
    PHENYL_LOGI(LOGGER, "Dumping level");
    LevelSerializable serializable{m_world, m_serializer};
    LevelMarker marker{};
    core::SerializeToJson(file, serializable, marker, true);
}

Level* LevelManager::load (Level&& obj, std::size_t id) {
    PHENYL_LOGD(LOGGER, "Loading already created level");
    m_levels[id] = std::make_unique<Level>(std::move(obj));
    return m_levels[id].get();
}

std::string_view LevelManager::getName () const noexcept {
    return "LevelManager";
}

Level::Level (std::ifstream file, LevelManager& manager) : m_file{std::move(file)}, m_manager{manager} {
    PHENYL_DASSERT(!this->m_file.bad());
    m_startPos = this->m_file.tellg();
}

void Level::loadImmediate (core::World& world, core::EntityComponentSerializer& serializer) {
    m_file.seekg(m_startPos);
    LevelSerializable serializable{world, serializer};
    LevelMarker marker{};
    core::DeserializeFromJson(m_file, serializable, marker);
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
    m_manager.queueLoad(assetFromThis(), additive);
}
