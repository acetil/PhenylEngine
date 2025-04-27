#pragma once

#include <iosfwd>
#include <memory>

#include "util/map.h"
#include "core/assets/asset.h"
#include "core/assets/asset_manager.h"
#include "core/entity.h"
#include "core/iresource.h"

#include "engine/level/level.h"

namespace phenyl::game {
    // TODO: manager interface
    class LevelManager : public core::AssetManager<Level>, public core::IResource {
    private:
        util::Map<std::size_t, std::unique_ptr<Level>> m_levels;
        core::World& m_world;
        core::EntityComponentSerializer& m_serializer;

        std::vector<core::Asset<Level>> m_queuedLoads;
        bool m_queuedClear = false;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
    protected:
        Level* load(std::ifstream& data, std::size_t id) override;
        Level* load (phenyl::game::Level&& obj, std::size_t id) override;
    public:
        LevelManager (core::World& manager, core::EntityComponentSerializer& serializer);
        ~LevelManager() override;

        [[nodiscard]] std::string_view getName() const noexcept override;

        void selfRegister ();

        void queueLoad (core::Asset<Level> level, bool additive);
        void loadLevels ();

        void dump (std::ostream& file) const;
    };
}