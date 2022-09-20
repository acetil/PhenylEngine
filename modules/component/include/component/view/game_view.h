#pragma once
#include "util/data.h"
#include "component/component.h"

namespace game {
    class GameCamera;
}

namespace game::detail {
    class GameObject;
}

namespace view {
    class GameView {
    private:
        game::detail::GameObject* gameObject;
    public:
        explicit GameView (game::detail::GameObject* _gameObject) :
            gameObject(_gameObject) {}
        component::EntityView createEntityInstance (const std::string& name, const util::DataValue& data = util::DataValue());
        void destroyEntityInstance (component::EntityId entityId);
        game::GameCamera& getCamera ();
    };
}

