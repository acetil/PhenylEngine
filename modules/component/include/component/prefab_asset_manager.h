#pragma once

#include "util/map.h"

#include "common/assets/asset_manager.h"
#include "component/prefab.h"
#include "forward.h"

namespace phenyl::component {
    class PrefabAssetManager : public common::AssetManager<Prefab> {
    private:
        util::Map<std::size_t, std::unique_ptr<Prefab>> prefabs;
        EntitySerializer& serializer;
        World& world;
    public:
        explicit PrefabAssetManager (World& world, EntitySerializer& serializer) : serializer{serializer}, world{world} {}
        ~PrefabAssetManager() override;

        Prefab* load (std::istream& data, std::size_t id) override;
        Prefab* load (phenyl::component::Prefab&& obj, std::size_t id) override;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;

        void selfRegister ();
        void clear ();
    };
}