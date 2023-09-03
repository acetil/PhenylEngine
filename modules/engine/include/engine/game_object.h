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
#include "engine/entity/entity_type.h"
#include "engine/entity/controller/entity_controller.h"
#include "component/prefab.h"

namespace view {
    class DebugGameView;

};

namespace game::detail {
    class ComponentSerialiser;
        class GameObject : public util::SmartHelper<GameObject, true>{
                private:
                util::Map<std::string, std::unique_ptr<game::EntityController>> controllers;
                std::map<std::string, int> tileMap;
                std::vector<Tile*> tileRegistry;
                event::EventBus::SharedPtr eventBus;
                component::EntityComponentManager* entityComponentManager;

                Map::SharedPtr gameMap;

                GameCamera camera;
                GameInput gameInput;

                event::EventScope eventScope;

                util::Map<std::string, EntityType> entityTypes;
                util::Map<std::string, component::Prefab> prefabs;
                component::EntitySerialiser* serialiser;

                component::Entity makeDeserialisedEntity (const util::DataValue& serialised);

                util::DataObject serialiseEntity (component::Entity& entityView);
                util::Optional<component::Prefab> makePrefab (const util::DataValue& val);


                public:
                ~GameObject();

                void registerEntityController (std::unique_ptr<EntityController> controller);

                util::Optional<component::Entity> createNewEntityInstance (const std::string& name, const util::DataValue& data = util::DataValue());

                void deleteEntityInstance (component::EntityId entityId);
                void registerTile (Tile* tile);
                int getTileId (const std::string& name);
                Tile* getTile (const std::string& name);
                Tile* getTile (int tileId);

                void updateEntityPosition ();
                void setEntityComponentManager (component::EntityComponentManager* manager);

                void updateEntitiesPrePhysics ();
                void updateEntitiesPostPhysics ();
                event::EventBus::SharedPtr getEventBus();

                util::Optional<EntityController*> getController (const std::string& name);

                void reloadMap ();
                void loadMap (Map::SharedPtr map);

                void dumpMap (const std::string& filepath);

                void mapReloadRequest (event::ReloadMapEvent& event);
                void mapDumpRequest (event::DumpMapEvent& event);
                void mapLoadRequest (event::MapLoadRequestEvent& event);

                void updateCamera (graphics::Camera& camera);

                void addEventHandlers (event::EventBus::SharedPtr eventBus);

                void setSerialiser (component::EntitySerialiser* serialiser);

                void addDefaultSerialisers ();

                void addEntityType (const std::string& typeId, const std::string& filepath);

                void clearPrefabs ();

                GameCamera& getCamera ();
                GameInput& getInput ();

                friend view::DebugGameView;
            };
    }
