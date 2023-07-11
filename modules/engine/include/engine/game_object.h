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
                component::EntityComponentManager::SharedPtr entityComponentManager;

                Map::SharedPtr gameMap;

                GameCamera camera;
                GameInput gameInput;

                event::EventScope eventScope;

                util::Map<std::string, EntityType> entityTypes;
                component::EntitySerialiser* serialiser;

                component::EntityView makeDeserialisedEntity (const util::DataValue& serialised);

                util::DataObject serialiseEntity (component::EntityView& entityView);

                public:
                ~GameObject();

                void registerEntityController (std::unique_ptr<EntityController> controller);

                util::Optional<component::EntityView> createNewEntityInstance (const std::string& name, const util::DataValue& data = util::DataValue());

                void deleteEntityInstance (component::EntityId entityId);
                void registerTile (Tile* tile);
                int getTileId (const std::string& name);
                Tile* getTile (const std::string& name);
                Tile* getTile (int tileId);

                void updateEntityPosition ();
                void setEntityComponentManager (component::EntityComponentManager::SharedPtr manager);

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

                GameCamera& getCamera ();
                GameInput& getInput ();

                friend view::DebugGameView;
            };
    }
