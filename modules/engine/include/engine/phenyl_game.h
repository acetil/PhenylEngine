#pragma once

#include <memory>
#include <string>
#include <utility>

#include "component/component.h"
#include "entity/controller/entity_controller.h"
#include "util/data.h"
#include "engine/input/game_input.h"

namespace graphics {
    class PhenylGraphics;
}

namespace game {
    namespace detail {
        class GameObject;
        class ComponentSerialiser {
        public:
            virtual ~ComponentSerialiser() = default;
            virtual bool deserialiseComp (component::view::EntityView& entityView, const util::DataValue& serialisedComp) = 0;
            virtual util::DataValue serialiseComp (component::view::EntityView& entityView) = 0;

            virtual bool hasComp (component::view::EntityView& entityView) = 0;
        };

        template <class T, typename SerialiseF, typename DeserialiseF>
        class ComponentSerialiserImpl : public ComponentSerialiser {
        private:
            SerialiseF serialiseF; // (const T&) -> util::DataValue
            DeserialiseF deserialiseF; // (const util::DataValue&) -> util::Optional<T>
        public:
            ComponentSerialiserImpl (SerialiseF f1, DeserialiseF f2) : serialiseF{f1}, deserialiseF{f2} {}

            util::DataValue serialiseComp (component::view::EntityView& entityView) override {
                return entityView.getComponent<T>()
                        .thenMap([this](T& comp) -> util::DataValue {
                            return serialiseF(comp);
                        })
                        .orElse({});
            }

             bool deserialiseComp (component::view::EntityView& entityView, const util::DataValue& serialisedComp) override {
                auto opt = deserialiseF(serialisedComp);

                opt.ifPresent([&entityView] (T& val) {
                    entityView.addComponent<T>(std::move(val));
                });

                return opt;
            }

            bool hasComp(component::view::EntityView& entityView) override {
                return entityView.hasComponent<T>();
            }
        };
    }
    class PhenylGame {
    private:
        std::weak_ptr<game::detail::GameObject> gameObject;
        [[nodiscard]] std::shared_ptr<game::detail::GameObject> getShared () const;
        void addComponentSerialiserInt (const std::string& component, std::unique_ptr<detail::ComponentSerialiser> serialiser);
    public:
        explicit PhenylGame (std::weak_ptr<detail::GameObject> _gameObject) : gameObject{std::move(_gameObject)} {
            addDefaultSerialisers();
        }

        void registerEntityType (const std::string& name, EntityTypeBuilder entityType);

        void buildEntityTypes ();

        component::EntityId createNewEntityInstance (const std::string& name, float x, float y, float rot=0.0f, const util::DataValue& data=util::DataValue());

        void deleteEntityInstance (component::EntityId entityId);
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

        void addEventHandlers (event::EventBus::SharedPtr eventBus);

        GameCamera& getCamera ();

        std::shared_ptr<game::detail::GameObject> tempGetPtr () {
            return getShared();
        }

        GameInput& getGameInput ();

        template <class T, typename F1, typename F2>
        void addComponentSerialiser (const std::string& component, F1 serialiseFunc, F2 deserialiseFunc) {
            addComponentSerialiserInt(component, std::make_unique<detail::ComponentSerialiserImpl<T, F1, F2>>(std::move(serialiseFunc), std::move(deserialiseFunc)));
        }

        void addDefaultSerialisers ();

        void addEntityType (const std::string& typeId, const std::string& filepath);
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

        void initGame (const graphics::PhenylGraphics& graphics, event::EventBus::SharedPtr eventBus);
    };
}