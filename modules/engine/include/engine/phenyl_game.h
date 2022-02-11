#pragma once

#include <memory>
#include <string>
#include <utility>

#include "component/component.h"
#include "entity/controller/entity_controller.h"
#include "util/data.h"

namespace graphics {
    class PhenylGraphics;
}

namespace game {
    namespace detail {
        class GameObject;
    }
    class PhenylGame {
    private:
        std::weak_ptr<game::detail::GameObject> gameObject;
        [[nodiscard]] std::shared_ptr<game::detail::GameObject> getShared () const;
    public:
        explicit PhenylGame (std::weak_ptr<detail::GameObject> _gameObject) : gameObject{std::move(_gameObject)} {}

        void registerEntityType (const std::string& name, EntityTypeBuilder entityType);

        void buildEntityTypes ();

        int createNewEntityInstance (const std::string& name, float x, float y, float rot=0.0f, const util::DataValue& data=util::DataValue());

        void deleteEntityInstance (int entityId);
        void registerTile (Tile* tile);
        int getTileId (const std::string& name);
        Tile* getTile (const std::string& name);
        Tile* getTile (int tileId);

        void updateEntityPosition ();

        void setTextureIds (graphics::TextureAtlas& atlas);

        void setEntityComponentManager (component::EntityComponentManager::SharedPtr compManager);

        void updateEntitiesPrePhysics ();
        void updateEntitiesPostPhysics ();

        event::EventBus::SharedPtr getEventBus ();

        std::shared_ptr<EntityController> getController (const std::string& name);

        void reloadMap ();
        void loadMap (Map::SharedPtr map);
        void dumpMap (const std::string& filepath);

        void mapReloadRequest (event::ReloadMapEvent& event);
        void mapDumpRequest (event::DumpMapEvent& event);
        void mapLoadRequest (event::MapLoadRequestEvent& event);

        void updateCamera (graphics::Camera& camera);

        GameCamera& getCamera ();

        std::shared_ptr<game::detail::GameObject> tempGetPtr () {
            return getShared();
        }
    };

    class PhenylGameHolder {
    private:
        std::shared_ptr<detail::GameObject> gameObject;
    public:
        PhenylGameHolder ();
        //explicit PhenylGameHolder (std::shared_ptr<detail::GameObject> _gameObject) : gameObject{std::move(_gameObject)} {};
        ~PhenylGameHolder();

        [[nodiscard]] PhenylGame getGameObject () const;

        [[nodiscard]] std::shared_ptr<detail::GameObject> tempGetGameObject () const {
            return gameObject;
        };

        void initGame (graphics::PhenylGraphics graphics);
    };
}