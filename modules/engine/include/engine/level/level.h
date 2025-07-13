#pragma once

#include "core/assets/asset.h"
#include "core/assets/load_context.h"
#include "core/serialization/component_serializer.h"

#include <fstream>

namespace phenyl::game {
namespace detail {
    class LevelEntity;
}

class LevelManager;

class Level : public core::Asset<Level>, public std::enable_shared_from_this<Level> {
public:
    void load (bool additive = false);

private:
    // std::vector<detail::LevelEntity> entities;
    core::AssetLoadContext m_loadCtx;
    LevelManager& m_manager;

    Level (core::AssetLoadContext loadCtx, LevelManager& manager);

    void loadImmediate (core::World& world, core::EntityComponentSerializer& serializer);
    friend LevelManager;
};
} // namespace phenyl::game
