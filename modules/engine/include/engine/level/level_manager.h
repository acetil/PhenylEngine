#pragma once

#include "core/assets/asset.h"
#include "core/assets/asset_manager.h"
#include "core/entity.h"
#include "core/iresource.h"
#include "engine/level/level.h"

#include <iosfwd>
#include <memory>

namespace phenyl::game {
// TODO: manager interface
class LevelManager : public core::AssetManager<Level>, public core::IResource {
public:
    LevelManager (core::World& manager, core::EntityComponentSerializer& serializer);
    ~LevelManager () override;

    [[nodiscard]] std::string_view getName () const noexcept override;

    void selfRegister ();

    void queueLoad (std::shared_ptr<Level> level, bool additive);
    void loadLevels ();

    void dump (std::ostream& file) const;

protected:
    Level* load (std::ifstream& data, std::size_t id) override;
    std::shared_ptr<Level> load2 (std::ifstream& data) override;
    Level* load (phenyl::game::Level&& obj, std::size_t id) override;

private:
    std::unordered_map<std::size_t, std::unique_ptr<Level>> m_levels;
    core::World& m_world;
    core::EntityComponentSerializer& m_serializer;

    std::vector<std::shared_ptr<Level>> m_queuedLoads;
    bool m_queuedClear = false;

    void queueUnload (std::size_t id) override;
    [[nodiscard]] const char* getFileType () const override;
};
} // namespace phenyl::game
