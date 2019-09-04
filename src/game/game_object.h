#include <vector>
#include <map>
#include <string>
#include "entity/entity.h"

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
namespace game {
    class GameObject {
        private:
        std::map<std::string, AbstractEntity*> entityRegistry;
        std::map<int, AbstractEntity*> entities;
        int currentEntityId = 0;
        public:
        void registerEntity (std::string name, AbstractEntity* entity);
        AbstractEntity* getEntity (std::string name);
        AbstractEntity* createNewEntityInstance (std::string name, float x, float y);
        AbstractEntity* getEntityInstance (int entityId);
        void deleteEntityInstance (AbstractEntity* entity);
        void deleteEntityInstance (int entityId);

        void updateEntities (float deltaTime);
        void updateEntityPosition (float deltaTime);
        void renderEntities (graphics::Graphics* graphics);
        
    };
}
#endif