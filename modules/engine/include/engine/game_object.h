#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include "engine/tile/tile.h"
#include "event/event_bus.h"
#include "component/component.h"
#include "util/smart_help.h"
#include "util/optional.h"
#include "engine/map/map.h"
#include "common/events/debug/reload_map.h"
#include "common/events/debug/dump_map.h"
#include "common/events/map_load_request.h"
#include "input/game_input.h"
#include "game_camera.h"
#include "component/prefab.h"
#include "component/component_serializer.h"

namespace view {
    class DebugGameView;

};

namespace game::detail {
    class ComponentSerialiser;
        class GameObject : public util::SmartHelper<GameObject, true>{
                private:
                std::map<std::string, int> tileMap;
                std::vector<Tile*> tileRegistry;
                event::EventBus::SharedPtr eventBus;
                component::EntityComponentManager* entityComponentManager;

                Map::SharedPtr gameMap;

                GameCamera camera;
                GameInput gameInput;

                event::EventScope eventScope;

                //util::Map<std::string, EntityType> entityTypes;
                util::Map<std::string, component::Prefab> prefabs;
                component::EntitySerializer* serializer;

                component::Entity makeDeserializedEntity (const nlohmann::json& serialized);

                nlohmann::json serializeEntity (component::Entity& entityView);
                util::Optional<component::Prefab> makePrefab (const nlohmann::json& val);


                public:
                ~GameObject();

                component::Entity createNewEntityInstance (const std::string& name);

                void deleteEntityInstance (component::EntityId entityId);
                void registerTile (Tile* tile);
                int getTileId (const std::string& name);
                Tile* getTile (const std::string& name);
                Tile* getTile (int tileId);

                void setEntityComponentManager (component::EntityComponentManager* manager);
                event::EventBus::SharedPtr getEventBus();

                void reloadMap ();
                void loadMap (Map::SharedPtr map);

                void dumpMap (const std::string& filepath);

                void mapReloadRequest (event::ReloadMapEvent& event);
                void mapDumpRequest (event::DumpMapEvent& event);
                void mapLoadRequest (event::MapLoadRequestEvent& event);

                void updateCamera (graphics::Camera& camera);

                void addEventHandlers (event::EventBus::SharedPtr eventBus);

                void setSerializer (component::EntitySerializer* serializer);

                void addDefaultSerialisers ();

                void addEntityType (const std::string& typeId, const std::string& filepath);

                void clearPrefabs ();

                GameCamera& getCamera ();
                GameInput& getInput ();

                friend view::DebugGameView;
            };
    }
