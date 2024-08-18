#pragma once

#include <vector>

#include "component/forward.h"

namespace phenyl::game {
    namespace detail {
        class LevelEntity;
    }

    class LevelManager;

    class Level {
    private:
        std::vector<detail::LevelEntity> entities;
        component::World* world;
        component::EntitySerializer* serializer;

        Level (component::World* world, component::EntitySerializer* serializer, std::vector<detail::LevelEntity> entities);

        component::Entity loadEntity (std::size_t index);
        friend LevelManager;
    public:
        void load (bool additive=false);
    };
}