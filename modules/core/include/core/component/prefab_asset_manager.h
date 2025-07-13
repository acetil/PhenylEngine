#pragma once

#include "core/assets/asset_manager.h"
#include "core/prefab.h"
#include "forward.h"

namespace phenyl::core {
class EntityComponentSerializer;

class PrefabAssetManager : public core::AssetManager<Prefab> {
public:
    explicit PrefabAssetManager (World& world, EntityComponentSerializer& serializer) :
        m_serializer{serializer},
        m_world{world} {}

    ~PrefabAssetManager () override;

    std::shared_ptr<Prefab> load (AssetLoadContext& ctx) override;

    void selfRegister ();

private:
    std::unordered_map<std::size_t, std::unique_ptr<Prefab>> m_prefabs;
    EntityComponentSerializer& m_serializer;
    World& m_world;
};
} // namespace phenyl::core
