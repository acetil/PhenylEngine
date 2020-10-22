#include <string>
#include <unordered_map>
#include <type_traits>
#include "event/event.h"
#include "graphics/maths_headers.h"
#include "entity.h"

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H
namespace game {
    #ifndef CONTROLLER_H
    class EntityController;
    #endif
    struct EntityType {
        EntityController* defaultController;

        glm::vec2 scale;
        glm::vec2 collisionScale;

        float defaultConstFriction;
        float defaultLinFriction;

        float defaultMass;

        unsigned int defaultLayers;
        unsigned int defaultCollisionMask;
        unsigned int defaultEventLayers;
        unsigned int defaultResolveLayers;

        AbstractEntity* (*entityFactory)();

        EntityType () = default;
    };

    class EntityTypeBuilder {
    private:
        glm::vec2 scale = {1.0f, 1.0f};
        glm::vec2 collScale = {1.0f, 1.0f};
        bool hasSetCollScale = false;
        float defaultConstFriction = 1.0f;
        float defaultLinFriction = 1.0f;
        float defaultMass = 1.0f;
        unsigned int defaultLayers = 0;
        unsigned int defaultCollisionMask = 0;
        unsigned int defaultEventLayers = 0;
        unsigned int defaultResolveLayers = 0;
        AbstractEntity* (*entityFactory)();
        std::string controller;
    public:
        EntityTypeBuilder () = default;
        EntityTypeBuilder (std::string _controller, AbstractEntity* (*_entityFactory)()) : entityFactory(_entityFactory),
            controller(std::move(_controller)) {};
        EntityTypeBuilder& setScale (float _scale);
        EntityTypeBuilder& setScale (float xScale, float yScale);
        EntityTypeBuilder& setCollisonScale (float _scale); // TODO: work with more complicated hitboxes
        EntityTypeBuilder& setCollisonScale (float xScale, float yScale);
        EntityTypeBuilder& setConstFriction (float friction);
        EntityTypeBuilder& setLinearFriction (float friction);
        EntityTypeBuilder& setMass (float mass);
        EntityTypeBuilder& addLayers (unsigned int layers);
        EntityTypeBuilder& addCollisionLayers (unsigned int layers);
        EntityTypeBuilder& addEventLayers (unsigned int layers);
        EntityTypeBuilder& addResolveLayers (unsigned int layers);
        EntityType build (const std::unordered_map<std::string, EntityController*>& controllerMap);
    };

    template <class T>
    T* constructor_factory () {
       return new T();
    }

    template <class T, class V>
    V* constructor_factory () {
        static_assert(std::is_base_of<V, T>::value, "First class must be derived from the second class!");
        return new T();
    }

}
#endif
