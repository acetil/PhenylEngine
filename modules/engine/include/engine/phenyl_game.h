#pragma once

#include <memory>
#include <string>
#include <utility>

#include "component/component.h"
#include "util/data.h"
#include "engine/input/game_input.h"

namespace graphics {
    class PhenylGraphics;
    class Camera;
}

namespace game {
    class GameCamera;
    namespace detail {
        class GameObject;
    }
    class PhenylGame {
    private:
        std::weak_ptr<game::detail::GameObject> gameObject;
        [[nodiscard]] std::shared_ptr<game::detail::GameObject> getShared () const;
    public:
        explicit PhenylGame (std::weak_ptr<detail::GameObject> _gameObject) : gameObject{std::move(_gameObject)} {}

        void setEntityComponentManager (component::EntityComponentManager* compManager);

        event::EventBus::SharedPtr getEventBus ();

        void updateCamera (graphics::Camera& camera);

        void addEventHandlers (event::EventBus::SharedPtr eventBus);

        GameCamera& getCamera ();

        std::shared_ptr<game::detail::GameObject> tempGetPtr () {
            return getShared();
        }

        GameInput& getGameInput ();

        void setSerializer (component::EntitySerializer* serialiser);

        void addDefaultSerialisers ();
    };

    class PhenylGameHolder {
    private:
        std::shared_ptr<detail::GameObject> gameObject;
    public:
        PhenylGameHolder ();
        ~PhenylGameHolder();

        [[nodiscard]] PhenylGame getGameObject () const;

        [[nodiscard]] std::shared_ptr<detail::GameObject> tempGetGameObject () const {
            return gameObject;
        };

        void initGame (const graphics::PhenylGraphics& graphics, event::EventBus::SharedPtr eventBus);
    };
}