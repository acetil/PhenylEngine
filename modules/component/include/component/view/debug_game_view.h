#include <utility>

#include "engine/game_object.h"

#ifndef DEBUG_GAME_VIEW_H
#define DEBUG_GAME_VIEW_H
namespace view {
    class DebugGameView {
    private:
        game::GameObject::SharedPtr gameObject;

    public:
        explicit DebugGameView(game::GameObject::SharedPtr _gameObject) : gameObject(std::move(_gameObject)) {}

        component::EntityComponentManager::SharedPtr getComponentManager () {
            return gameObject->entityComponentManager;
        }

    };
}
#endif