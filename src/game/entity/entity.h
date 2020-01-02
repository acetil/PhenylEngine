#include <string>
#include "physics/physics.h"
#include "physics/physics_body.h"
#include "graphics/textures/texture_atlas.h"
#include "graphics/graphics.h"

#ifndef ENTITY_H
#define ENTITY_H
namespace game {
    class AbstractEntity {
        friend class GameObject;
        protected:
        float* x;
        float* y;
        float scale;
        physics::PhysicsBody* physicsBody;
        std::string name;
        int textureId;
        int entityId;
        public:
        virtual AbstractEntity* createEntity (float x, float y) = 0;
        virtual int getEntityId ();
        virtual std::string getEntityName ();
        
        virtual void addForce (physics::Force force);
        virtual void clearMovementForces (int tag);
        virtual void update (float time);
        virtual void updatePosition (float time);
        virtual void setVelocity (float velX, float velY);

        virtual void setTextureIds (graphics::Graphics* graphics) = 0;
        virtual void render (graphics::Graphics* graphics);

        virtual void setEntityId (int entityId);
    };
}
#endif