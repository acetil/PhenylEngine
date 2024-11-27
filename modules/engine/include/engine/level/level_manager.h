#pragma once

#include <iosfwd>
#include <memory>

#include "util/map.h"
#include "common/assets/asset.h"
#include "common/assets/asset_manager.h"
#include "component/entity.h"
#include "runtime/iresource.h"

#include "engine/level/level.h"

namespace phenyl::game {
    // TODO: manager interface
    class LevelManager : public common::AssetManager<Level>, public runtime::IResource {
    private:
        util::Map<std::size_t, std::unique_ptr<Level>> levels;
        component::World& world;
        component::EntityComponentSerializer& serializer;

        std::vector<common::Asset<Level>> queuedLoads;
        bool queuedClear = false;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
    protected:
        Level* load(std::ifstream& data, std::size_t id) override;
        Level* load (phenyl::game::Level&& obj, std::size_t id) override;
    public:
        LevelManager (component::World& manager, component::EntityComponentSerializer& serializer);
        ~LevelManager() override;

        [[nodiscard]] std::string_view getName() const noexcept override;

        void selfRegister ();

        void queueLoad (common::Asset<Level> level, bool additive);
        void loadLevels ();

        void dump (std::ostream& file) const;
    };
}