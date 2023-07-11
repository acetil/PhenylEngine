#pragma once
#include "util/data.h"
#include "component/component.h"
#include "component/component_new.h"

namespace game {
    class GameCamera;
}

namespace game::detail {
    class GameObject;
}

namespace physics {
    class IPhysics;
}

namespace view {
    class GameView {
    private:
        game::detail::GameObject* gameObject;
        physics::IPhysics* physics;
    public:
        explicit GameView (game::detail::GameObject* _gameObject/*, physics::IPhysics* _physics*/) :
            gameObject(_gameObject), physics{nullptr} {}
        component::EntityView createEntityInstance (const std::string& name, const util::DataValue& data = util::DataValue());
        void destroyEntityInstance (component::EntityId entityId);
        game::GameCamera& getCamera ();
        physics::IPhysics* getPhysics ();
    };
}

