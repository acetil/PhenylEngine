#include <vector>
#include <map>
#include <string>
#include "entity/entity.h"
#include "graphics/textures/texture_atlas.h"
#include "tile/tile.h"
#include "event/event.h"
#include "component/component.h"
#include "component/main_component.h"

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
namespace game {
    class GameObject {
        private:
        std::map<std::string, AbstractEntity*> entityRegistry;
        //std::map<int, AbstractEntity*> entities;
        std::map<std::string, int> tileMap;
        std::vector<Tile*> tileRegistry;
        //int currentEntityId = 0;
        event::EventBus* eventBus = new event::EventBus();
        component::EntityComponentManager* entityComponentManager;
        public:
        ~GameObject();

        void registerEntity (const std::string& name, AbstractEntity* entity);

        [[maybe_unused]] AbstractEntity* getEntity (const std::string& name);
        AbstractEntity* createNewEntityInstance (const std::string& name, float x, float y);
        /*AbstractEntity* getEntityInstance (int entityId);
        void deleteEntityInstance (AbstractEntity* entity);*/

        void deleteEntityInstance (int entityId);
        void registerTile (Tile* tile);
        int getTileId (const std::string& name);
        Tile* getTile (const std::string& name);
        Tile* getTile (int tileId);

        //void updateEntities (float deltaTime);
        void updateEntityPosition ();
        //void updateEntityPositions (float deltaTime);
        void setTextureIds (graphics::TextureAtlas atlas);
        //void renderEntities (graphics::Graphics* graphics);
        void setEntityComponentManager (component::EntityComponentManager* manager);

        void updateEntitiesPrePhysics ();
        void updateEntitiesPostPhysics ();
        event::EventBus* getEventBus();
    };
}
#endif