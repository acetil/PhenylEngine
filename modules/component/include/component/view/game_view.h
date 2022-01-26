#include "engine/game_object.h"
#include "util/data.h"
#ifndef GAME_VIEW_H
#define GAME_VIEW_H
namespace view {
    class GameView {
    private:
        game::GameObject* gameObject;
    public:
        explicit GameView (game::GameObject* _gameObject) :
            gameObject(_gameObject) {}
        int createEntityInstance (const std::string& name, float x, float y, float rot = 0.0f, const util::DataValue& data = util::DataValue()) {
            return gameObject->createNewEntityInstance(name, x, y, rot, data);
        }
        void destroyEntityInstance (int entityId) {
            gameObject->deleteEntityInstance(entityId);
        }
        game::GameCamera& getCamera () {
            return gameObject->getCamera();
        }
    };
}
#endif //GAME_VIEW_H
