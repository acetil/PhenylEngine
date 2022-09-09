#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include "engine/entity/entity.h"
#include "graphics/textures/texture_atlas.h"
#include "engine/tile/tile.h"
#include "event/event_bus.h"
#include "component/component.h"
#include "component/main_component.h"
#include "engine/entity/entity_type.h"
#include "util/smart_help.h"
#include "util/optional.h"
#include "engine/map/map.h"
#include "common/events/debug/reload_map.h"
#include "common/events/debug/dump_map.h"
#include "common/events/map_load_request.h"
#include "input/game_input.h"
#include "game_camera.h"

namespace view {
    class DebugGameView;

};

namespace game::detail {
    class ComponentSerialiser;
        class GameObject : public util::SmartHelper<GameObject, true>/*, public std::enable_shared_from_this<GameObject>*/ {
                private:
                std::map<std::string, AbstractEntity*> entityRegistry;
                std::unordered_map<std::string, EntityType> entityTypes;
                std::unordered_map<std::string, std::shared_ptr<EntityController>> controllers;
                std::unordered_map<std::string, EntityTypeBuilder> entityTypeBuilders;
                //std::map<int, AbstractEntity*> entities;
                std::map<std::string, int> tileMap;
                std::vector<Tile*> tileRegistry;
                //int currentEntityId = 0;
                event::EventBus::SharedPtr eventBus;
                component::EntityComponentManager::SharedPtr entityComponentManager;

                Map::SharedPtr gameMap;

                GameCamera camera;
                GameInput gameInput;

                event::EventScope eventScope;

                util::Map<std::string, std::unique_ptr<ComponentSerialiser>> serialiserMap;

                component::EntityId deserialiseEntity (const std::string& type, float x, float y, float rot, const util::DataValue& serialised = util::DataValue());

                void deserialiseEntity2 (component::view::EntityView& entityView, const util::DataValue& entityData);
                util::DataObject serialiseEntity (component::view::EntityView& entityView);

                public:
                ~GameObject();

                //void registerEntity (const std::string& name, AbstractEntity* entity);
                void registerEntityType (const std::string& name, EntityTypeBuilder entityTypeBuilder);
                template <typename T>
                void registerEntityController (const std::string& name) {
                    static_assert(std::is_base_of_v<EntityController, T>, "Type must be child of EntityController!");
                    controllers[name] = std::make_shared<T>();
                }
                void buildEntityTypes ();
                //[[maybe_unused]] AbstractEntity* getEntity (const std::string& name);

                component::EntityId createNewEntityInstance (const std::string& name, float x, float y, float rot = 0.0f, const util::DataValue& data = util::DataValue());
                /*AbstractEntity* getEntityInstance (int entityId);
                void deleteEntityInstance (AbstractEntity* entity);*/

                void deleteEntityInstance (component::EntityId entityId);
                void registerTile (Tile* tile);
                int getTileId (const std::string& name);
                Tile* getTile (const std::string& name);
                Tile* getTile (int tileId);

                //void updateEntities (float deltaTime);
                void updateEntityPosition ();
                //void updateEntityPositions (float deltaTime);
                void setTextureIds (graphics::TextureAtlas& atlas);
                //void renderEntities (graphics::Graphics* graphics);
                void setEntityComponentManager (component::EntityComponentManager::SharedPtr manager);

                void updateEntitiesPrePhysics ();
                void updateEntitiesPostPhysics ();
                event::EventBus::SharedPtr getEventBus();

                std::shared_ptr<EntityController> getController (const std::string& name);

                void reloadMap ();
                void loadMap (Map::SharedPtr map);

                void dumpMap (const std::string& filepath);

                void mapReloadRequest (event::ReloadMapEvent& event);
                void mapDumpRequest (event::DumpMapEvent& event);
                void mapLoadRequest (event::MapLoadRequestEvent& event);

                void updateCamera (graphics::Camera& camera);

                void addEventHandlers (event::EventBus::SharedPtr eventBus);

                void addComponentSerialiser (const std::string& component, std::unique_ptr<ComponentSerialiser> serialiser);

                GameCamera& getCamera ();
                GameInput& getInput ();

                friend view::DebugGameView;
            };
    }
