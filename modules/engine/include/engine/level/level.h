#pragma once

#include <fstream>
#include <vector>

#include "component/component_serializer.h"

namespace phenyl::game {
    namespace detail {
        class LevelEntity;
    }

    class LevelManager;

    class Level : public common::IAssetType<Level> {
    private:
        //std::vector<detail::LevelEntity> entities;
        std::ifstream file;
        std::ifstream::pos_type startPos;
        LevelManager& manager;

        Level (std::ifstream file, LevelManager& manager);

        void loadImmediate (component::World& world, component::EntityComponentSerializer& serializer);
        friend LevelManager;
    public:
        void load (bool additive=false);
    };
}