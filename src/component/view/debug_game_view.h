#include "game/game_object.h"

#ifndef DEBUG_GAME_VIEW_H
#define DEBUG_GAME_VIEW_H
namespace view {
    class DebugGameView {
    private:
        game::GameObject* gameObject;

    public:
        explicit DebugGameView(game::GameObject* _gameObject) : gameObject(_gameObject) {}

        component::EntityComponentManager* getComponentManager () {
            return gameObject->entityComponentManager;
        }

    };
}
#endif