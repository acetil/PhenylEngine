#pragma once

#include "common/assets/asset_manager.h"
#include "component/prefab.h"
#include "forward.h"

namespace component {
    class PrefabManager : public common::AssetManager<Prefab> {
    private:
        util::Map<std::size_t, std::unique_ptr<Prefab>> prefabs;
        EntitySerializer* serializer;
        ComponentManager* manager;
    public:
        explicit PrefabManager (ComponentManager* manager, EntitySerializer* serializer) : serializer{serializer}, manager{manager} {}
        ~PrefabManager() override;

        Prefab* load (std::istream& data, std::size_t id) override;
        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;

        void selfRegister ();
        void clear ();
    };
}