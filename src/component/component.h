#include <unordered_map>
#include <string>
#include <vector>

#include "game/entity/entity.h"
#include "event/event.h"
#ifndef MAX_COMPONENT_ENTITIES
#define MAX_COMPONENT_ENTITIES 256
#endif
#ifndef COMPONENT_H
#define COMPONENT_H
#if MAX_COMPONENT_ENTITIES <= 256
namespace component {
    typedef unsigned char id_type_t;
}
#else
namespace component {
    typedef unsigned short id_type_t;
}
#endif
unsigned char a;
namespace component {
    struct Component {
        void* data;
        size_t size;
    };
    class ComponentManager {
        private:
        std::unordered_map<std::string, int> componentIds;
        std::vector<Component> components;
        id_type_t numEntities;
        id_type_t maxEntities;
        event::EventBus* bus;
        public:
        ComponentManager(id_type_t maxEntities, event::EventBus* bus);
        ~ComponentManager();
        template <typename T>
        void applyFunc (void func(T*, int, int), int compId) {
            func((T*)components[compId].data, numEntities, 1);
        };
        template <typename T>
        void applyFuncAsync (void func(T*, int, int), int compId, int numThreads) {
            applyFunc(func, compId); // TODO: update
        }
        template <typename T>
        int addComponent (std::string name) {
            if (componentIds.count(name) > 0) {
                return -1;
            }
            components.push_back({new T[maxEntities], sizeof(T)});
            componentIds[name] = components.size() - 1;
            return componentIds[name];
        }
        template <typename T>
        T* getComponent (int compId) {
            return (T*) components[compId].data;
        }
        template <typename T>
        T* getComponent (std::string name) {
            return getComponent<T>(componentIds[name]);
        }
        template <typename T>
        T getEntityData (int compId, int entityId) {
            // not the recommended way to access component data, use applyFunc where possible
            return ((T*)components[compId].data)[entityId];
        };
        id_type_t getNumEntities ();
        id_type_t addEntity (game::AbstractEntity* entity);
        void removeEntity (id_type_t entityId);
    };
}
#endif