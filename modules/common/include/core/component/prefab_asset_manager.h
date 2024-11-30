#pragma once

#include "util/map.h"

#include "core/assets/asset_manager.h"
#include "core/prefab.h"
#include "forward.h"

namespace phenyl::core {
    class EntityComponentSerializer;

    class PrefabAssetManager : public core::AssetManager<Prefab> {
    private:
        util::Map<std::size_t, std::unique_ptr<Prefab>> prefabs;
        EntityComponentSerializer& serializer;
        World& world;
    public:
        explicit PrefabAssetManager (World& world, EntityComponentSerializer& serializer) : serializer{serializer}, world{world} {}
        ~PrefabAssetManager() override;

        Prefab* load (std::ifstream& data, std::size_t id) override;
        Prefab* load (phenyl::core::Prefab&& obj, std::size_t id) override;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;

        void selfRegister ();
        void clear ();
    };
}