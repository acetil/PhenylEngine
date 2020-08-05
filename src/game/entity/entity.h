#include <string>
#include "graphics/textures/texture_atlas.h"

#ifndef ENTITY_H
#define ENTITY_H
namespace game {
    // TODO: change AbstractEntity to be more like "EntityType", and move most other stuff to EntityController if possible
    class EntityController;
    class AbstractEntity {
        friend class GameObject;
        protected:
        float* x;
        float* y;
        float scale;
        std::string name;
        int textureId;
        int entityId;
        EntityController* controller;
        public:
        static std::string getObjectType () {
            return "entity";
        }
        virtual ~AbstractEntity() = default;
        virtual AbstractEntity* createEntity () = 0;
        virtual int getEntityId ();
        virtual std::string getEntityName ();

        virtual void update (float time);

        virtual void setTextureIds (graphics::TextureAtlas& graphics) = 0;
        //virtual void render (graphics::Graphics* graphics);
        virtual int getTextureId ();
        virtual void setEntityId (int entityId);
        virtual EntityController* getController ();
    };
}
#endif