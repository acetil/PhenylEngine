#pragma once

#include <fstream>
#include <vector>

#include "core/serialization/component_serializer.h"

namespace phenyl::game {
    namespace detail {
        class LevelEntity;
    }

    class LevelManager;

    class Level : public core::IAssetType<Level> {
    public:
        void load (bool additive=false);

    private:
        //std::vector<detail::LevelEntity> entities;
        std::ifstream m_file;
        std::ifstream::pos_type m_startPos;
        LevelManager& m_manager;

        Level (std::ifstream file, LevelManager& manager);

        void loadImmediate (core::World& world, core::EntityComponentSerializer& serializer);
        friend LevelManager;
    };
}