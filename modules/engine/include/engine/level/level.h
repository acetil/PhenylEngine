#pragma once

#include <fstream>
#include <vector>

#include "component/component_serializer.h"

namespace phenyl::game {
    namespace detail {
        class LevelEntity;
    }

    class LevelManager;

    class Level {
    private:
        //std::vector<detail::LevelEntity> entities;
        std::ifstream file;
        std::ifstream::pos_type startPos;
        component::World& world;
        component::EntityComponentSerializer& serializer;

        Level (std::ifstream file, component::World& world, component::EntityComponentSerializer& serializer);
        friend LevelManager;
    public:
        void load (bool additive=false);
    };
}