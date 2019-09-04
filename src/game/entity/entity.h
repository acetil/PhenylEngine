#include <string>
#include "physics/physics.h"
#include "physics/physics_body.h"
#include "graphics/textures/texture_atlas.h"
#include "graphics/graphics.h"

#ifndef ENTITY_H
#define ENTITY_H
namespace game {
    class AbstractEntity {
        protected:
        float x;
        float y;
        float scale;
        physics::PhysicsBody* physicsBody;
        std::string name;
        int textureId;
        int entityId;
        public:
        virtual AbstractEntity* createEntity (float x, float y) = 0;
        virtual void setEntityId (int entityId);
        virtual int getEntityId ();

        virtual void addForce (physics::Force force);
        virtual void clearMovementForces ();
        virtual void updatePosition (float time);
        virtual void setVelocity (float velX, float velY);

        virtual void setTextureIds (graphics::TextureAtlas* atlas) = 0;
        virtual void render (graphics::Graphics* graphics);

    };
}
#endif