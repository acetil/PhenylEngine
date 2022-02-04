#pragma once
#include <utility>

#include "engine/game_object.h"
#include "game_object.h"

namespace view {
    class DebugGameView {
    private:
        game::detail::GameObject::SharedPtr gameObject;

    public:
        explicit DebugGameView(game::detail::GameObject::SharedPtr _gameObject) : gameObject(std::move(_gameObject)) {}

        component::EntityComponentManager::SharedPtr getComponentManager () {
            return gameObject->entityComponentManager;
        }

    };
}
