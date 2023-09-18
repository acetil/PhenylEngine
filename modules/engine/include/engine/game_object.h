#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include "event/event_bus.h"
#include "component/component.h"
#include "util/smart_help.h"
#include "util/optional.h"
#include "common/events/debug/dump_map.h"
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
                event::EventBus::SharedPtr eventBus;
                component::EntityComponentManager* entityComponentManager;

                //Map::SharedPtr gameMap;

                GameCamera camera;
                GameInput gameInput;

                event::EventScope eventScope;

                //util::Map<std::string, EntityType> entityTypes;
                //util::Map<std::string, component::Prefab> prefabs;
                component::EntitySerializer* serializer;
                //util::Optional<component::Prefab> makePrefab (const nlohmann::json& val);


                public:
                ~GameObject();

                //component::Entity createNewEntityInstance (const std::string& name);

                //void deleteEntityInstance (component::EntityId entityId);

                void setEntityComponentManager (component::EntityComponentManager* manager);
                event::EventBus::SharedPtr getEventBus();

                void updateCamera (graphics::Camera& camera);

                void addEventHandlers (event::EventBus::SharedPtr eventBus);

                void setSerializer (component::EntitySerializer* serializer);

                void addDefaultSerialisers ();

                GameCamera& getCamera ();
                GameInput& getInput ();

                friend view::DebugGameView;
            };
    }
