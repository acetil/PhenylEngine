#include "game/game_object.h"
#ifndef GAME_VIEW_H
#define GAME_VIEW_H
namespace view {
    class GameView {
    private:
        game::GameObject* gameObject;
    public:
        explicit GameView (game::GameObject* _gameObject) :
            gameObject(_gameObject) {}
        int createEntityInstance (const std::string& name, float x, float y) {
            return gameObject->createNewEntityInstance(name, x, y);
        }
        void destroyEntityInstance (int entityId) {
            gameObject->deleteEntityInstance(entityId);
        }
    };
}
#endif //GAME_VIEW_H
