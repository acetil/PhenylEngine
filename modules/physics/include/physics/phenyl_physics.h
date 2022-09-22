#pragma once

#include <memory>

#include "component/forward.h"
#include "event/forward.h"

namespace view {
    class GameView;
}

namespace physics {
    class IPhysics;
    class PhenylPhysics {
    private:
        IPhysics* physicsObj;
    public:
        explicit PhenylPhysics (IPhysics* physicsObj);
        void updatePhysics (const std::shared_ptr<component::EntityComponentManager>& componentManager);
        void checkCollisions (const std::shared_ptr<component::EntityComponentManager>& componentManager, const std::shared_ptr<event::EventBus>& eventBus, view::GameView& gameView);

        void updateEntityPosition (const std::shared_ptr<component::EntityComponentManager>& componentManager, const std::shared_ptr<event::EventBus>& eventBus, view::GameView& gameView);
    };
}