#pragma once

#include "core/assets/asset.h"
#include "core/serialization/component_serializer.h"

#include <fstream>

namespace phenyl::game {
namespace detail {
    class LevelEntity;
}

class LevelManager;

class Level : public core::AssetType<Level>, public std::enable_shared_from_this<Level> {
public:
    void load (bool additive = false);

private:
    // std::vector<detail::LevelEntity> entities;
    std::ifstream m_file;
    std::ifstream::pos_type m_startPos;
    LevelManager& m_manager;

    Level (std::ifstream file, LevelManager& manager);

    void loadImmediate (core::World& world, core::EntityComponentSerializer& serializer);
    friend LevelManager;
};
} // namespace phenyl::game
