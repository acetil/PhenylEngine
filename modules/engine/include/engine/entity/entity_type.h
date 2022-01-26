#pragma once

#include <string>
#include <unordered_map>
#include <type_traits>
#include <memory>
#include <functional>
#include <utility>

#include "event/event_bus.h"
#include "graphics/maths_headers.h"
#include "entity.h"

#include "component/serialisable_component.h"

namespace game {
    #ifndef CONTROLLER_H
    class EntityController;
    #endif
    struct EntityType : component::SerialisableComponent<EntityType>{
    private:
        static constexpr std::string_view compName = "entity_type";
    public:
        std::shared_ptr<EntityController> defaultController{};

        std::string controller{};

        glm::vec2 scale{};
        glm::vec2 collisionScale{};

        float defaultConstFriction{};
        float defaultLinFriction{};

        float defaultMass{};

        unsigned int defaultLayers{};
        unsigned int defaultCollisionMask{};
        unsigned int defaultEventLayers{};
        unsigned int defaultResolveLayers{};

        //AbstractEntity* (*entityFactory)();
        std::function<AbstractEntity*(void)> entityFactory{};

        std::string typeName;

        EntityType () = default;
        util::DataValue serialise () {
            util::DataObject obj;
            obj["controller"] = controller;
            obj["scale"] = scale;
            obj["collision_scale"] = collisionScale;
            obj["default_const_friction"] = defaultConstFriction;
            obj["default_lin_friction"] = defaultLinFriction;
            obj["default_mass"] = defaultMass;
            obj["default_layers"] = defaultLayers;
            obj["default_coll_mask"] = defaultCollisionMask;
            obj["default_event_layers"] = defaultEventLayers;
            obj["default_resolve_layers"] = defaultResolveLayers;
            obj["type_name"] = typeName;
            return (util::DataValue)obj;
        }

        void deserialise (const util::DataValue& val) {
            auto obj = val.get<util::DataObject>();

            controller = obj.at("controller").get<std::string>();
            scale = obj.at("scale").get<glm::vec2>();
            collisionScale = obj.at("collision_scale").get<glm::vec2>();
            defaultConstFriction = obj.at("default_const_friction").get<float>();
            defaultLinFriction = obj.at("default_lin_friction").get<float>();
            defaultMass = obj.at("default_mass").get<float>();
            defaultLayers = obj.at("default_layers").get<int>();
            defaultCollisionMask = obj.at("default_coll_mask").get<int>();
            defaultEventLayers = obj.at("default_event_layers").get<int>();
            defaultResolveLayers = obj.at("default_resolve_layers").get<int>();
            typeName = obj.at("type_name").get<std::string>();
        }

        static constexpr std::string_view const& getName () {
            return compName;
        }
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
        //AbstractEntity* (*entityFactory)();
        std::function<AbstractEntity*(void)> entityFactory;
        std::string controller;
        std::string typeName = "";
    public:
        EntityTypeBuilder () = default;
        EntityTypeBuilder (const std::string& _typeName, std::string _controller, std::function<AbstractEntity*(void)> _entityFactory) : entityFactory(std::move(_entityFactory)),
            controller(std::move(_controller)), typeName(_typeName) {};
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
        EntityType build (const std::unordered_map<std::string, std::shared_ptr<EntityController>>& controllerMap);
    };

    template <class T>
    T* constructor_factory () {
       return new T();
    }

    template <class V, class T>
    V* constructor_factory () {
        static_assert(std::is_base_of<V, T>::value, "First class must be derived from the second class!");
        return new T();
    }

}
