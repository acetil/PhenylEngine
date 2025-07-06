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
    std::shared_ptr<Level> load (std::ifstream& data) override;

private:
    core::World& m_world;
    core::EntityComponentSerializer& m_serializer;

    std::vector<std::shared_ptr<Level>> m_queuedLoads;
    bool m_queuedClear = false;

    [[nodiscard]] const char* getFileType () const override;
};
} // namespace phenyl::game
