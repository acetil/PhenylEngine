#pragma once

#include "util/map.h"

#include "core/assets/asset_manager.h"
#include "core/prefab.h"
#include "forward.h"

namespace phenyl::core {
    class EntityComponentSerializer;

    class PrefabAssetManager : public core::AssetManager<Prefab> {
    public:
        explicit PrefabAssetManager (World& world, EntityComponentSerializer& serializer) : m_serializer{serializer}, m_world{world} {}
        ~PrefabAssetManager() override;

        Prefab* load (std::ifstream& data, std::size_t id) override;
        Prefab* load (phenyl::core::Prefab&& obj, std::size_t id) override;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;

        void selfRegister ();
        void clear ();

    private:
        util::Map<std::size_t, std::unique_ptr<Prefab>> m_prefabs;
        EntityComponentSerializer& m_serializer;
        World& m_world;
    };
}